#!/usr/bin/env python3
"""strict_audit.py - compare every accepted function of the C files of sld (../*.c) with the original in the strict mode of fncheck.

Usage: strict_audit.py [NAME ...]

Each C file is compiled alone with scc -O; each function is cut out of its object by its symbol and compared with the code
range of the original (own_functions.txt) after fncheck.norm and keep_locals (frame offsets are compared, link
addresses are masked). Prints per function: the status of the table, the instruction counts and the number of
differing instructions after a sequence alignment, so functions that were accepted with the old loose check but
differ in frame offsets or parameter registers show up.
"""
import difflib
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fncheck as F  # noqa: E402
from sout import SOut  # noqa: E402


def main():
    import ldsrc
    objs, errs = ldsrc.compile_all('-O')
    if errs:
        sys.exit('does not compile:\n' + errs)
    codes, ours = {}, {}
    for f, obj in objs.items():
        o = SOut(open(obj, 'rb').read())
        tsz = o.segs[0]['code']
        codes[f] = bytes(o.image[:tsz])
        starts = sorted((x['value'], x['name'].lstrip('_')) for x in o.symbols if x['type'] == 34 and x['segt'] == 0)
        for i, (v, n) in enumerate(starts):
            ours[n] = (f, v, starts[i + 1][0] if i + 1 < len(starts) else tsz)
    s = SOut(open(F.ORIG, 'rb').read())
    cs = s.segs[0]['code']
    ocode = bytes(s.image[:cs])
    want = sys.argv[1:]
    rows = [l.split() for l in open(os.path.join(HERE, 'own_functions.txt')) if l.strip() and not l.startswith('#')]
    for p in rows:
        entry, name, end, status = int(p[0], 16), p[1], int(p[2], 16), p[-1]
        if status == 'open' or (want and name not in want) or name not in ours:
            continue
        f, lo, hi = ours[name]
        A = F.keep_locals([F.norm(t) for _, t in F.insns(codes[f], lo, hi, True)])
        B = F.keep_locals([F.norm(t) for _, t in F.insns(ocode, entry, end, (s.magic >> 8) == 0xE6)])
        sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
        diff = sum(max(i2 - i1, j2 - j1) for tag, i1, i2, j1, j2 in sm.get_opcodes() if tag != 'equal')
        print('%-16s %-14s mine %3d orig %3d differ %3d%s' % (name, status, len(A), len(B), diff,
                                                              '' if diff == 0 else '  <--'))
        if want and diff:
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag == 'equal':
                    continue
                for t in A[i1:i2]:
                    print('    mine ' + t)
                for t in B[j1:j2]:
                    print('    orig ' + t)
                print('    --')


if __name__ == '__main__':
    main()
