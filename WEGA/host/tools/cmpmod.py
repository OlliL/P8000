#!/usr/bin/env python3
"""cmpmod.py: compare a module compiled by scc with the same code in a segmented binary.

    cmpmod.py OBJECT.o BINARY START END [-v] [-a]

OBJECT.o   the module, compiled with scc (functions in source order)
BINARY     the original program (segmented, e.g. /bin/cas)
START END  text range of the module in the binary (hex)
-v         print the differing instructions of every function
-a         align by the function sequence even if the counts differ
-f         compare only the fingerprint (calls, constants, variables), tolerant
           against the other code generator
-s         strict: do not rename scratch registers, keep frame sizes and save slots

Both sides are reduced to a normal form: addresses, symbols, labels and branch
targets are masked; stack offsets of the local variables and constants are kept.
Unless -s is given, also masked: the frame size, the save slots of the register
variables, and the scratch registers r0-r5 are not distinguished (the older
compiler used other scratch registers and other frame sizes).
The scc of the shipped system puts the prologue behind the body ("jr Lp ... Lp:
dec r15 ... jr Lb"); the older compiler that built most WEGA programs has it
in front. The object is converted to the second form before comparing.
"""
import sys, re, os, subprocess, difflib, collections
here = os.path.dirname(os.path.abspath(__file__))

def sdis(path, *opts):
    return subprocess.run([sys.executable, os.path.join(here, 'sdis.py')] + list(opts) + [path],
                          capture_output=True, text=True).stdout.split('\n')

def norm(t):
    t = t.strip()
    m = re.match(r'(\w+)\s*(.*)$', t)
    if not m:
        return t
    op, rest = m.group(1), m.group(2)
    if op in ('call', 'calr'):
        return op + ' A'
    if op in ('jr', 'jp') :
        cc = rest.split(',')[0] if ',' in rest else ''
        if rest.startswith('@'):
            return op + ' ' + rest
        return '%s %s' % (op, cc)
    if op == 'djnz' or op == 'dbjnz':
        return op + ' ' + rest.split(',')[0]
    # stack offsets
    rest = re.sub(r'\|<<127>>%([0-9A-F]{4})\|\(r15\)', lambda m: 'S%d(r15)' % int(m.group(1), 16), rest)
    rest = re.sub(r'<<127>>%([0-9A-F]{4})\(r15\)', lambda m: 'S%d(r15)' % int(m.group(1), 16), rest)
    rest = re.sub(r'\|_stkseg(?:\+(\d+))?\|\(r15\)', lambda m: 'S%d(r15)' % int(m.group(1) or 0), rest)
    rest = re.sub(r'_stkseg(?:\+(\d+))?\(r15\)', lambda m: 'S%d(r15)' % int(m.group(1) or 0), rest)
    rest = re.sub(r'#%FF0000([0-9A-F]{2})', lambda m: '#S%d' % int(m.group(1), 16), rest)
    rest = re.sub(r'#_stkseg(?:\+(\d+))?', lambda m: '#S%d' % int(m.group(1) or 0), rest)
    # addresses and symbols
    rest = re.sub(r'#%8[0-9A-F]00[0-9A-F]{4}\b', '#A', rest)
    rest = re.sub(r'<<\d+>>%[0-9A-F]{4}', 'A', rest)
    rest = re.sub(r'#(?:_\w+|L1d_[0-9a-f]+)(?:[+-]\d+)?', '#A', rest)
    rest = re.sub(r'(?<![\w%])_\w+(?:[+-]\d+)?', 'A', rest)
    rest = re.sub(r'L(?:0t|1d)_[0-9a-f]+', 'L', rest)
    return (op + ' ' + rest).strip()

def fingerprint(seq):
    """the parts of the code that do not depend on the code generator: calls, constants,
    stack slots and data addresses (as a multiset)"""
    out = []
    for t in seq:
        op = t.split(' ')[0]
        if op in ('call', 'calr', 'ret') or re.search(r'#|\bA\b|S\d+\(r15\)|\bA\(', t):
            if op in ('ld', 'ldl', 'ldb', 'lda') and re.search(r'^\w+ (R|RR|RL|RH|r\d+|rr\d+|rl\d+|rh\d+),(S\d+\(r15\)|A)$', t):
                out.append(op[:2] + ' mem')		# a load from a variable
                continue
            if op in ('ld', 'ldl', 'ldb', 'lda') and re.search(r',(R|RR|RL|RH|r\d+|rr\d+|rl\d+|rh\d+)$', t) and re.search(r'^\w+ (S\d+\(r15\)|A)', t):
                out.append(op[:2] + ' store')
                continue
            if re.match(r'(dec|inc|sub|add) r15|frame|ldm', t):
                continue
            out.append(t)
    return out

def relax(seq):
    """mask frame size and save slots, rename scratch register pairs by first use"""
    out = []
    def reg(m):
        kind, n = m.group(1), int(m.group(2))
        if n > 5 and not (kind in ('rl', 'rh') and n > 5):
            return m.group(0)
        if kind in ('rr', 'rq') and n > 4 and kind == 'rr':
            return m.group(0)
        if n > 5:
            return m.group(0)
        return kind.upper()
    for t in seq:
        if re.match(r'(dec|inc|sub|add) r15,', t):
            t = 'frame'
        elif re.match(r'ldm (S\d+\(r15\),r(?:8|9|1\d)|r(?:8|9|1\d),S\d+\(r15\))', t):
            t = re.sub(r'S\d+\(r15\)', 'Sx(r15)', t)
        elif re.match(r'ldl? (r(?:r)?(?:8|9|1\d)),S\d+\(r15\)$', t) or re.match(r'ldl? S\d+\(r15\),r(?:r)?(?:8|9|1\d)$', t):
            t = re.sub(r'S\d+\(r15\)', 'Sx(r15)', t)
        t = re.sub(r'\b(rr|rh|rl|r)(\d+)\b', reg, t)
        out.append(t)
    return out

def parse_obj(path):
    funcs, cur, body, seg = [], None, [], None
    for l in sdis(path):
        if l.startswith('// segment table entry'):
            seg = 'text' if 'text' in l else 'data'
            if cur and seg != 'text':
                funcs.append((cur, body)); cur = None
            continue
        if seg != 'text':
            continue
        m = re.match(r'^(_\w+)::', l)
        if m:
            if cur: funcs.append((cur, body))
            cur, body = m.group(1), []
        elif cur and l.startswith(('\t', 'L0t_')):
            body.append(l)
    if cur: funcs.append((cur, body))
    return funcs

def unhop(body):
    """convert 'jr Lp; body; Lp: prologue; jr Lb' into 'prologue; body'."""
    ins = [l for l in body]
    if not ins or not re.match(r'\t(jr|jp)\tL0t_', ins[0]):
        return ins
    lp = ins[0].split('\t')[2]
    try:
        k = ins.index(lp + ':')
    except ValueError:
        return ins
    # prologue: from k+1 to the jr back to the label that follows the first jr
    j = k + 1
    while j < len(ins) and not re.match(r'\t(jr|jp)\tL0t_', ins[j]):
        j += 1
    if j >= len(ins):
        return ins
    lb = ins[j].split('\t')[2]
    # the label right after the first jr must be lb
    if len(ins) < 2 or ins[1] != lb + ':':
        return ins
    return ins[k+1:j] + ins[1:k]

def obj_norm(body):
    out = []
    lines = unhop(body)
    for i, l in enumerate(lines):
        if not l.startswith('\t'):
            continue
        m = re.match(r'\tjr\t(L0t_[0-9a-f]+)$', l)
        if m and i + 1 < len(lines) and lines[i + 1] == m.group(1) + ':':
            continue			# jump to the next instruction (return)
        out.append(norm(l))
    return out

def parse_bin(path, lo, hi):
    ins, seg = [], None
    for l in sdis(path, '-l'):
        if l.startswith('// segment table entry'):
            seg = 'text' if 'text' in l else 'data'
            continue
        if seg != 'text':
            continue
        m = re.match(r'([0-9a-f]{4}) [0-9a-f ]+?\t(.*)$', l)
        if m:
            a = int(m.group(1), 16)
            if lo <= a < hi:
                ins.append((a, m.group(2)))
    return ins

def split_bin(ins):
    """functions: a new one starts after a ret that is not followed by a branch target inside"""
    funcs, cur = [], []
    prev_ret = False
    for a, t in ins:
        if prev_ret and cur:
            funcs.append(cur); cur = []
        cur.append((a, t))
        prev_ret = t.startswith('ret') and not t.startswith('ret\t') or t == 'ret'
    if cur: funcs.append(cur)
    return funcs

def main(argv):
    verbose = '-v' in argv
    args = [a for a in argv if not a.startswith('-')]
    obj, binary, lo, hi = args[0], args[1], int(args[2], 16), int(args[3], 16)
    strict = '-s' in argv
    of = [(n, obj_norm(b)) for n, b in parse_obj(obj)]
    if not strict:
        of = [(n, relax(b)) for n, b in of]
    bf = split_bin(parse_bin(binary, lo, hi))
    print('%d functions in the object, %d in the binary range %04x-%04x' % (len(of), len(bf), lo, hi))
    tot = bad = 0
    for i in range(max(len(of), len(bf))):
        n = of[i][0] if i < len(of) else '-'
        a = bf[i][0][0] if i < len(bf) else None
        A = of[i][1] if i < len(of) else []
        B = [norm(t) for _, t in bf[i]] if i < len(bf) else []
        if not strict:
            B = relax(B)
        sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
        d = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != 'equal')
        tot += 1
        if d: bad += 1
        same_set = d and collections.Counter(A) == collections.Counter(B)
        fpa, fpb = collections.Counter(fingerprint(A)), collections.Counter(fingerprint(B))
        same_fp = d and not same_set and fpa == fpb
        if same_set or same_fp:
            bad -= 1
        print('%-14s %s %4d/%4d instr  %s' % (n, ('%04x' % a) if a is not None else '----', len(A), len(B),
                                                 'identical' if not d else
                                                 'same instructions, other order (switch?)' if same_set else
                                                 'same fingerprint (calls, constants, variables)' if same_fp else
                                                 '%d differing, fingerprint differs by %d' % (d, sum(((fpa - fpb) + (fpb - fpa)).values()))))
        if verbose and d and not same_set and not same_fp:
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag == 'equal': continue
                for k in range(i1, i2): print('    new ', A[k])
                for k in range(j1, j2): print('    orig', B[k] + '   @%04x' % bf[i][k][0])
    print('%d of %d functions differ' % (bad, tot))

if __name__ == '__main__':
    main(sys.argv[1:])
