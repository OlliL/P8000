#!/usr/bin/env python3
"""firstdiff.py NAME [N] - the first N (default 1) differing places of a function against the original, with 6 lines of context
on both sides (mine / orig); uses the strict normalisation of strict_audit.py"""
import difflib
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import fncheck as F  # noqa: E402
import ldsrc  # noqa: E402
from sout import SOut  # noqa: E402


def main():
    name = sys.argv[1]
    n = int(sys.argv[2]) if len(sys.argv) > 2 else 1
    objs, errs = ldsrc.compile_all('-O')
    if errs:
        sys.exit(errs)
    ours = None
    for f, obj in objs.items():
        o = SOut(open(obj, 'rb').read())
        tsz = o.segs[0]['code']
        starts = sorted((x['value'], x['name'].lstrip('_')) for x in o.symbols if x['type'] == 34 and x['segt'] == 0)
        for i, (v, nm) in enumerate(starts):
            if nm == name:
                ours = (bytes(o.image[:tsz]), v, starts[i + 1][0] if i + 1 < len(starts) else tsz)
    s = SOut(open(F.ORIG, 'rb').read())
    ocode = bytes(s.image[:s.segs[0]['code']])
    ent = end = None
    for l in open(os.path.join(HERE, 'own_functions.txt')):
        p = l.split()
        if len(p) > 2 and p[1] == name:
            ent, end = int(p[0], 16), int(p[2], 16)
    A = F.keep_locals([F.norm(t) for _, t in F.insns(ours[0], ours[1], ours[2], True)])
    B = F.keep_locals([F.norm(t) for _, t in F.insns(ocode, ent, end, (s.magic >> 8) == 0xE6)])
    sm = difflib.SequenceMatcher(None, A, B, autojunk=False)
    shown = 0
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            continue
        print('--- mine %d..%d / orig %d..%d (%s)' % (i1, i2, j1, j2, tag))
        for t in A[max(0, i1 - 6):i1]:
            print('      ' + t)
        for t in A[i1:min(i2, i1 + 25)]:
            print('  mine ' + t)
        for t in B[j1:min(j2, j1 + 25)]:
            print('  orig ' + t)
        shown += 1
        if shown >= n:
            break


if __name__ == '__main__':
    main()
