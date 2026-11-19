#!/usr/bin/env python3
"""seglibfit.py [-v] PROGRAM SEG:ADDR SOURCE.c [-D name=val ...]: which segmented compiler makes the member of an old slibc from a C source.

Compiles SOURCE.c with every segmented compiler (the shipped /bin/scc of WEGA and the old ones of OLDROOT: sccvc sccv1 sccv1c sccvk sccvl scc,
with -O and without) and compares the code of the object, instruction by instruction (addresses, call targets and numbers masked), with
the code of PROGRAM at SEG:ADDR for as many bytes as the object has. Prints per compiler the number of differing instructions (0 = fits;
the bytes of the code can still differ in the addresses); -v shows the diff of the best one.
"""
import difflib
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', 'recon'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402

WEGA = os.environ.get('WEGA', '/home/olivleh1/git/P8000/tmp/wega31_root')
OLDROOT = os.environ.get('OLDROOT', '/home/olivleh1/git/P8000/tmp/oldroot')
EMU = os.path.normpath(os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu'))
TMP = os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp')
COMPILERS = [('scc', WEGA, '/bin/scc')] + [(c, OLDROOT, '/bin/' + c) for c in ('sccvc', 'sccv1', 'sccv1c', 'sccvk', 'sccvl', 'scc')]
CODESEGS = {4: (0, 65534), 5: (65534, 50432)}    # gopt; others: pass -s SEG=OFF,SIZE


def nm(t):
    t = zcode.norm_masked(t)
    t = re.sub(r'<<\d+>>', '', t)
    t = re.sub(r'^(call|calr)\t.*', 'call X', t)
    t = re.sub(r'%[0-9A-Fa-f]{2,4}\b', 'N', t)
    return re.sub(r'\|?ADDR\|?', 'A', t)


def listing(code, lo, hi):
    return [(pc, nm(t)) for pc, t in zcode.insns(code, lo, hi, True)]


def main():
    a = sys.argv[1:]
    verbose = False
    defs = ['-DSEG', '-I.']
    while a and a[0].startswith('-'):
        if a[0] == '-v':
            verbose = True
            a = a[1:]
        elif a[0] == '-D':
            defs.append('-D' + a[1])
            a = a[2:]
        else:
            break
    prog, where, src = a[0], a[1], a[2]
    sg, ad = where.split(':')
    sg, ad = int(sg), int(ad, 16)
    s = SOut(open(prog, 'rb').read())
    off, n = CODESEGS[sg]
    img = bytes(s.image[off:off + n])
    results = []
    for name, root, cc in COMPILERS:
        for fl in ('-O', ''):
            w = tempfile.mkdtemp(dir=TMP)
            shutil.copy(src, w)
            base = os.path.splitext(os.path.basename(src))[0]
            for h in os.listdir(os.path.dirname(os.path.abspath(src))):
                if h.endswith('.h'):
                    shutil.copy(os.path.join(os.path.dirname(os.path.abspath(src)), h), w)
            r = subprocess.run([EMU, '-r', root, cc] + ([fl] if fl else []) + defs + ['-c', base + '.c'], cwd=w, capture_output=True, text=True, timeout=600)
            obj = os.path.join(w, base + '.o')
            if not os.path.exists(obj):
                results.append((9999, name, fl, (r.stdout + r.stderr)[:80], None, None))
                shutil.rmtree(w, ignore_errors=True)
                continue
            o = SOut(open(obj, 'rb').read())
            code = bytes(o.image[:o.segs[0]['code']])
            mine = listing(code, 0, len(code))
            orig = listing(img, ad, ad + len(code))
            sm = difflib.SequenceMatcher(None, [t for _, t in orig], [t for _, t in mine], autojunk=False)
            d = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != 'equal')
            results.append((d, name, fl, '%d bytes' % len(code), orig, mine))
            shutil.rmtree(w, ignore_errors=True)
    results.sort(key=lambda r: r[0])
    for d, name, fl, info, _, _ in results:
        print('%4d  %-7s %-2s %s' % (d, name, fl or '-', info))
    if verbose and results[0][4] is not None:
        d, name, fl, info, orig, mine = results[0]
        for l in difflib.unified_diff([t for _, t in orig], [t for _, t in mine], 'gopt', name, lineterm='', n=1):
            print(l)


if __name__ == '__main__':
    main()
