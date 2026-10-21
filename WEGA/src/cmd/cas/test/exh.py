#!/usr/bin/env python3
"""Exhaustive assembler test: every valid Z8000 opcode (decoded by tools/z8k.py) is written as
cas source, assembled by the original cas and by the cas under test, and compared.
usage: WEGA=<tree> exh.py ns|ss|sl|fz|fzs --new <cas under test> [--limit N]
ns, ss, sl: all opcodes (nonsegmented, short and long segmented); fz, fzs: operand patterns for the
mnemonics that z8k.py does not decode (EPU, block instructions, ...), nonsegmented and segmented.
Differences between the two cas are listed as NEW; the other lines say where the text of z8k.py is
not accepted by the original cas (a difference of syntax, not of the assembler)."""
import subprocess, sys, re, collections, os, tempfile, shutil
import os
HERE = os.path.dirname(os.path.abspath(__file__))
HOST = os.path.join(HERE, '..', '..', '..', '..', 'host')
ROOT = os.environ.get('WEGA')                 # file tree of the original system
if not ROOT:
    raise SystemExit('set WEGA=<file tree of the original system>')
EMU = [os.path.join(HOST, 'z8k-emu', 'z8k-emu'), '-r', ROOT]

NATIVE = bool(os.environ.get('NATIVE'))      # the cas under test is a program of this host (host/cas), not a WEGA binary
def runcmd(prog):
    """the command that runs prog: the original (/bin/cas) under the emulator, a WEGA binary under test too,
    a native program (NATIVE=1) directly"""
    return [prog] if NATIVE and prog != '/bin/cas' else EMU + [prog]
ORIG = '/bin/cas'                             # the original, run by the emulator
sys.path.insert(0, os.path.join(HOST, 'tools'))
import z8k
from sout import SOut
FILL = {'ns': [0x1234, 0x5678, 0x9ABC], 'ss': [0x0134, 0x5678, 0x9ABC], 'sl': [0x8100, 0x1234, 0x5678]}

def gen(mode):
    seg = mode != 'ns'
    out = []
    for w0 in range(0x10000):
        data = b''.join(w.to_bytes(2, 'big') for w in [w0] + FILL[mode] + [0x1111])
        ins = z8k.decode(z8k.Buf(data), 0, seg)
        if ins is None or ins.mn == 'epu':
            continue
        if any(o.kind == 'rel' for o in ins.ops):
            continue                      # relative operands: tested separately (labels)
        mask = bytearray(b'\xff' * (2 * len(ins.words)))
        for o in ins.ops:
            if o.kind == 'imm' and o.b == 8 and o.word is not None:
                mask[2 * o.word] = 0          # the high byte of a byte immediate is not defined
        out.append((b''.join(w.to_bytes(2, 'big') for w in ins.words), z8k.text(ins), bytes(mask)))
    return out

POOL = ['r5','r0','rh3','rl3','rr4','rq4','@r5','@rr4','#5','#%1234','#1','#%12','%1234','|%12|','%1234(r5)','r5(#%1234)','rr4(#%1234)','r5(r6)','rr4(r6)','eq','ne','nz','c','z','f3','f0','fcw','vi','nvi','x','x(r5)','#x','x+2']
P3 = ['r5','@r5','@rr4','#3','eq','rr4','f2']
P4 = ['r5','@r5','eq','#3']

def genfuzz(mode):
    import itertools, re as _re
    rows = _re.findall(r'\{ (\d+), (\d+), (\d+), [^,]+, "([^"]+)" \}', open(os.path.join(HERE, '..', 'pstdata.h')).read())
    skip = {51, 52, 53, 54, 55, 58, 59, 60, 61}
    names = [(n, int(t)) for t, b, k, n in rows if not n.startswith('.') and int(t) not in skip]
    out = []
    P2 = POOL[:26]
    for n, t in names:
        pats = [()] + [(a,) for a in POOL] + list(itertools.product(P2, repeat=2)) + list(itertools.product(P3, repeat=3))
        if t == 56: pats += list(itertools.product(P4, repeat=4))
        for p in pats:
            out.append((b'', n + ('\t' + ','.join(p) if p else ''), b''))
    return out

def run(cas, lines, mode, tag):
    """assemble lines; drop the lines that give errors; returns (image, errors{line:msg}, kept indexes)"""
    errs = {}
    keep = list(range(len(lines)))
    for _ in range(4):
        src = ('.seg\n' if mode != 'ns' else '') + '.code\n' + ''.join('\t' + lines[i] + '\n' for i in keep)
        open('ch_%s.s' % tag, 'w').write(src)
        r = subprocess.run(runcmd(cas) +[ '-u', '-o', 'ch_%s.o' % tag, 'ch_%s.s' % tag], capture_output=True, text=True, timeout=300)
        off = 2 if mode != 'ns' else 1
        bad = {}
        for m in re.finditer(r'line (\d+): (.*)', r.stderr):
            ln = int(m.group(1)) - off - 1
            if 0 <= ln < len(keep): bad.setdefault(keep[ln], m.group(2))
        if not bad:
            break
        errs.update(bad)
        keep = [i for i in keep if i not in bad]
    try: img = bytes(SOut(open('ch_%s.o' % tag, 'rb').read()).image)
    except Exception: img = b''
    return img, errs, keep, r.stderr

FZ = sys.argv[1].startswith('fz')

def main():
    mode = sys.argv[1]
    src = os.path.abspath(sys.argv[sys.argv.index('--new') + 1]) if '--new' in sys.argv else None
    os.chdir(tempfile.mkdtemp())
    new = None
    if src:
        shutil.copy(src, 'cas.new')          # the guest sees only ROOT and the working directory
        new = './cas.new'
    lim = int(sys.argv[sys.argv.index('--limit') + 1]) if '--limit' in sys.argv else None
    items = genfuzz(mode[2:] or 'ns') if mode.startswith('fz') else gen(mode)
    mode = 'ns' if mode == 'fz' else ('sl' if mode == 'fzs' else mode)
    if lim: items = items[:lim]
    print(mode, 'opcodes', len(items))
    CH = 3000
    stat = collections.Counter(); ex = {}
    for c in range(0, len(items), CH):
        ch = items[c:c + CH]
        lines = [t for h, t, m in ch]
        img, errs, keep, _ = run(ORIG, lines, mode, 'o')
        for i, m in errs.items():
            k = (lines[i].split()[0], 'orig error: ' + m); stat[k] += 1; ex.setdefault(k, (ch[i][0].hex(), lines[i]))
        if FZ: stat[('accepted', 'by orig')] += len(keep)
        pos = 0
        for i in ([] if FZ else keep):
            want = ch[i][0]
            got = img[pos:pos + len(want)]; pos += len(want)
            msk = ch[i][2]
            if bytes(a & b for a, b in zip(got, msk)) != bytes(a & b for a, b in zip(want, msk)) or len(got) != len(want):
                k = (lines[i].split()[0], 'bytes differ'); stat[k] += 1; ex.setdefault(k, (want.hex(), lines[i], got.hex()))
        if new:
            img2, errs2, keep2, e2 = run(new, lines, mode, 'n')
            if errs2 != errs or keep2 != keep:
                stat[('NEW', 'other errors than orig in chunk %d' % c)] += 1
                for i in sorted(set(errs2) ^ set(errs))[:3]: ex.setdefault(('NEW', 'err line %s' % lines[i]), (errs.get(i), errs2.get(i)))
            if img2 != img:
                stat[('NEW', 'differs from orig in chunk %d' % c)] += 1
                for i, m in list(errs2.items())[:5]: ex.setdefault(('NEW', 'err %s' % lines[i]), m)
                if img2 != img:
                    # find first differing instruction
                    p = 0
                    for j, i in enumerate(keep):
                        n = len(ch[i][0])
                        if FZ: break
                        if img[p:p + n] != img2[p:p + n]:
                            ex.setdefault(('NEW', 'first diff chunk %d' % c), (ch[i][0].hex(), lines[i], img[p:p+n].hex(), img2[p:p+n].hex())); break
                        p += n
    for k, v in sorted(stat.items()): print(k, v, ex.get(k, ''))
    for k, v in ex.items():
        if k[0] == 'NEW': print(k, v)
if __name__ == '__main__':
    main()
