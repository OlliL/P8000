#!/usr/bin/env python3
"""blocks.py - shows the differing blocks of one function (normalised as in patterns.py), for the analysis of
reorderings: each block says what the original has (orig) and what we have (ours) at that place.

Usage: python3 blocks.py FUNC --mine ./codgen --orig $WEGA/lib/codgen [--n 8]
"""
import argparse
import difflib
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import funcs as F  # noqa: E402
import patterns as P  # noqa: E402
from sout import SOut  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('func')
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/codgen'))
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    ap.add_argument('--n', type=int, default=8)
    a = ap.parse_args()
    f = json.load(open(a.findings))['functions'][a.func]
    m = F.disasm(a.mine, F.own_limit(SOut(open(a.mine, 'rb').read())))
    o = F.disasm(a.orig)
    oend = P.orig_end_fn(o)
    st, en = [int(x, 16) for x in f['ours'].split('-')]
    g = int(f['orig'], 16)
    A = [P.dn(x) for x in F.extent(m, st, en)]
    B = [P.dn(x) for x in F.extent(o, g, oend(g))]
    ops = [op for op in difflib.SequenceMatcher(None, B, A, autojunk=False).get_opcodes() if op[0] != 'equal']
    print('%s: ours %d, orig %d instructions, %d differing blocks' % (a.func, len(A), len(B), len(ops)))
    for t, i1, i2, j1, j2 in sorted(ops, key=lambda x: -max(x[2] - x[1], x[4] - x[3]))[:a.n]:
        print('--- %s  orig[%d:%d]  ours[%d:%d]' % (t, i1, i2, j1, j2))
        print('   orig: ' + ' | '.join(B[i1:min(i2, i1 + 6)]))
        print('   ours: ' + ' | '.join(A[j1:min(j2, j1 + 6)]))


if __name__ == '__main__':
    main()
