#!/usr/bin/env python3
"""fill_strings.py - put the string literals of /bin/sld into the C files (replaces the placeholders "x").

Usage: fill_strings.py [--write]     (without --write: report only)

For every C file: (1) the file is compiled to assembler text; the string literals are the .byte items of the data section, in
the order of the literals in the source. (2) Every function of ours refers to its literals as #L<n> in the order of its code; the
original function (table own_functions.txt) refers to its strings as #%0100XXXX in the order of its code. The k-th string reference
of ours is the k-th string reference of the original. A literal that already has text must equal the original text (check of the
pairing). (3) With --write the placeholders get the original text.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import ldsrc  # noqa: E402
import unlink_seg as U  # noqa: E402
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
LD = os.path.join(HERE, '..')


VARS = set()


def parse_asm(text):
    """-> (items: [(label, bytes)] strings of the data section in order, funcs: {name: [labels referenced in order]})"""
    items, funcs = [], {}
    sect, cur, buf, label = None, None, None, None
    lines = text.split('\n')
    str_labels = {}
    i = 0
    while i < len(lines):
        l = lines[i].strip()
        if l.startswith('.psec'):
            sect = l.split()[1] if len(l.split()) > 1 else 'code'
        m = re.match(r'^(L\d+|_sccsid):$', l)
        if sect == 'data' and m:
            label, buf = m.group(1), []
            j = i + 1
            ok = True
            while j < len(lines) and not re.match(r'^(L\d+|_\w+):$', lines[j].strip()) and not lines[j].strip().startswith('.psec'):
                lj = lines[j].strip()
                if lj.startswith('.byte'):
                    buf += [int(x.strip().lstrip('%'), 16) for x in lj[5:].split(',')]
                elif lj.startswith('.word') or lj.startswith('.long'):
                    ok = False
                j += 1
            if ok and buf and buf[-1] == 0:
                items.append((label, bytes(buf)))
                str_labels[label] = len(items) - 1
        i += 1
    # code: function bodies
    cur = None
    for l in lines:
        t = l.strip()
        m = re.match(r'^_(\w+)::$', t)
        if m:
            cur = m.group(1)
            funcs[cur] = []
            continue
        if t.startswith('}\t/* _'):
            cur = None
            continue
        if cur:
            for m in re.finditer(r'#(L\d+|_\w+)\b', t):
                if m.group(1) in str_labels:
                    funcs[cur].append(m.group(1))
                elif m.group(1).startswith('_') and m.group(1)[1:] in VARS:
                    funcs[cur].append(m.group(1))
    return items, funcs


def source_literals(text):
    """string literals of the C source in order: [(start, end, decoded bytes)]"""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if text.startswith('/*', i):
            i = text.index('*/', i) + 2
        elif c == '#' and (i == 0 or text[i - 1] == '\n'):
            i = text.index('\n', i)
        elif c == "'":
            i += 1
            while text[i] != "'":
                i += 2 if text[i] == '\\' else 1
            i += 1
        elif c == '"':
            j = i + 1
            b = bytearray()
            while text[j] != '"':
                if text[j] == '\\':
                    j += 1
                    e = text[j]
                    if e in '01234567':
                        k = j
                        while k < j + 3 and text[k] in '01234567':
                            k += 1
                        b.append(int(text[j:k], 8) & 0xff)
                        j = k
                        continue
                    b.append({'n': 10, 't': 9, 'r': 13, 'b': 8, 'f': 12, '\\': 92, '"': 34, "'": 39}.get(e, ord(e)))
                else:
                    b.append(ord(text[j]))
                j += 1
            out.append((i, j + 1, bytes(b) + b'\0'))
            i = j + 1
        else:
            i += 1
    return out


def c_literal(b):
    s = ''
    for k, x in enumerate(b):
        ch = chr(x)
        if ch == '\n':
            s += '\\n'
        elif ch == '\t':
            s += '\\t'
        elif ch == '"':
            s += '\\"'
        elif ch == '\\':
            s += '\\\\'
        elif 32 <= x < 127:
            s += ch
        else:
            s += '\\%03o' % x
    return '"' + s + '"'


def original_strings():
    s = SOut(open(PROG, 'rb').read())
    code = s.segs[0]['code']
    prog = bytes(s.image[:code])
    data = bytes(s.image[code:])
    return s, prog, data


def orig_refs(prog, data, lo, hi):
    """string references (#%0100XXXX) of the original function in code order: data offsets"""
    out = []
    for pc, i in U.decode_range(prog, True, lo, hi):
        t = U.z8k.text(i)
        for m in re.finditer(r'#%0([12])00([0-9A-F]{4})', t):
            o = int(m.group(2), 16)
            out.append(o if m.group(1) == '1' else -1)
    return out


STRS = set()


def string_starts(data):
    st, i, n = set(), 0, len(data)
    while i < n:
        j = i
        while j < n and (32 <= data[j] < 127 or data[j] in (9, 10)):
            j += 1
        if j - i >= 2 and j < n and data[j] == 0:
            for k in range(i, j):
                st.add(k)
            i = j + 1
        else:
            i = j + 1 if j == i else j
    return st


def cstring_at(data, o):
    e = data.index(b'\0', o)
    return data[o:e + 1]


def add_sccs(prog_data):
    """every C file starts its data with the SCCS string (what string) of the file: static char sccsid[]"""
    for f, lo, hi in ldsrc.modules():
        path = os.path.join(LD, f)
        text = open(path).read()
        if 'sccsid' in text:
            continue
        m = re.search(rb'@\[\$\]' + re.escape(f.encode()) + rb'[^\x00]*', prog_data)
        lit = c_literal(m.group(0))
        text = text.replace('#include "ld.h"\n', '#include "ld.h"\n\nstatic char sccsid[] = %s;\n' % lit, 1)
        open(path, 'w').write(text)
        print('sccsid in', f)


def main():
    write = '--write' in sys.argv
    s, prog, data = original_strings()
    if '--sccs' in sys.argv:
        add_sccs(data)
        return
    global STRS
    STRS = string_starts(data)
    rows = {}
    for l in open(os.path.join(HERE, 'own_functions.txt')):
        if l[0] == '#' or not l.strip():
            continue
        p = l.split()
        rows[p[1]] = (int(p[0], 16), int(p[2], 16))
    for l in open(os.path.join(LD, 'ld.h')):
        if l.startswith('extern '):
            m = re.search(r'([A-Za-z_]\w*)\s*(\[[^\]]*\]\s*)*;$', re.sub(r'\{[^}]*\}', '', l))
            VARS.add(m.group(1))
    outs, errs = ldsrc.compile_all('-O', asm=True)
    if errs:
        sys.exit(errs)
    total = {'placeholders': 0, 'filled': 0, 'checked': 0, 'mismatch': 0, 'count_mismatch': 0}
    for f, lo, hi in ldsrc.modules():
        path = os.path.join(LD, f)
        text = open(path).read()
        items, funcs = parse_asm(open(outs[f]).read())
        lits = source_literals(text)
        if len(lits) != len(items):
            print('%s: %d literals in the source, %d data items' % (f, len(lits), len(items)))
        label_text = {}
        idx_of = {lab: k for k, (lab, _) in enumerate(items)}
        for (a, b, v), (lab, it) in zip(lits, items):
            if v != it.rstrip(b'\0') + b'\0':
                print('%s: literal %r differs from the data item %r' % (f, v, it))
            label_text[lab] = v
        new = {}
        pairs = {}
        for name, labs in funcs.items():
            if name not in rows:
                continue
            refs = orig_refs(prog, data, *rows[name])
            # primary: the literals of a function are consecutive in the source and the original keeps the source order in the
            # data: the k-th literal of the function gets the k-th lowest string address of the original function
            sl0 = [l for l in labs if l.startswith('L')]
            sr0 = sorted({o for o in refs if 0 <= o < len(data) and o in STRS})
            if sl0 and len(sl0) == len(sr0) and len(sl0) == len(set(sl0)):
                order_labs = sorted(sl0, key=lambda l: idx_of[l])
                for lab, o in zip(order_labs, sr0):
                    orig = cstring_at(data, o)
                    mine = label_text.get(lab)
                    pairs.setdefault(lab, set()).add(o)
                    if mine in (b'x\0', b'y\0', b'z\0'):
                        total['placeholders'] += 1
                        new[lab] = orig
                    else:
                        total['checked'] += 1
                        if mine != orig:
                            total['mismatch'] += 1
                            print('%s %s: literal %r against original %r (source order)' % (f, name, mine, orig))
                            if '--fix' in sys.argv:
                                new[lab] = orig
                continue
            if len(refs) != len(labs):
                # second try: only the references to strings (printable text ended by 0, 2 characters or more)
                sl = [l for l in labs if l.startswith('L')]
                sr = [o for o in refs if 0 <= o < len(data) and o in STRS]
                if len(sl) == len(sr):
                    labs, refs = sl, sr
                else:
                    total['count_mismatch'] += 1
                    ph = [l for l in sl if label_text.get(l) in (b'x\0', b'y\0', b'z\0')]
                    print('%s %s: %d literals (%d placeholders) against %d string references in the original: %s' % (
                        f, name, len(sl), len(ph), len(sr), [cstring_at(data, o) for o in sr]))
                    continue
            for lab, o in zip(labs, refs):
                if not lab.startswith('L') or o < 0 or o >= len(data):
                    continue
                orig = cstring_at(data, o)
                pairs.setdefault(lab, set()).add(o)
                mine = label_text.get(lab)
                if mine in (b'x\0', b'y\0', b'z\0'):
                    total['placeholders'] += 1
                    if lab in new and new[lab] != orig:
                        print('%s %s: label %s gets two texts' % (f, name, lab))
                    new[lab] = orig
                else:
                    total['checked'] += 1
                    if mine != orig:
                        total['mismatch'] += 1
                        print('%s %s: literal %r against original %r' % (f, name, mine, orig))
        # the strings of a file are in the order of the literals of the source: the addresses must increase
        idx = {lab: k for k, (lab, _) in enumerate(items)}
        seq = sorted((idx[l], min(o)) for l, o in pairs.items())
        for (k1, o1), (k2, o2) in zip(seq, seq[1:]):
            if o2 < o1:
                print('%s: ORDER literal %d (%r at 0x%x) is followed by literal %d (%r at 0x%x)' % (
                    f, k1, items[k1][1][:30], o1, k2, items[k2][1][:30], o2))
        if write:
            order = [lab for lab, _ in items]
            pos = {lab: k for k, lab in enumerate(order)}
            edits = []
            for lab, orig in new.items():
                a, b, v = lits[pos[lab]]
                edits.append((a, b, c_literal(orig[:-1])))
            for a, b, lit in sorted(edits, reverse=True):
                text = text[:a] + lit + text[b:]
                total['filled'] += 1
            open(path, 'w').write(text)
    print(total)


if __name__ == '__main__':
    main()
