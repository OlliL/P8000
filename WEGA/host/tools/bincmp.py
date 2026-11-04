#!/usr/bin/env python3
"""bincmp.py ORIG NEW [-v]: how far a built program is from the original.

Prints IDENTICAL, or the sizes, the number of differing bytes, the first difference in code and data, and the blocks of
instructions that differ with addresses masked (the source or compiler differences). With no such blocks only addresses differ:
look at the size of code and data, the bss order and the link order. -v: also the addresses that differ, most frequent first
(new -> original: a variable in another place). Strip NEW first when the original is stripped; symbols are not compared.
"""
import collections
import difflib
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recon'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402


def norm(t):
    return re.sub(r'%[0-9A-Fa-f]{1,8}\b', 'A', t)


def main():
    verbose = '-v' in sys.argv
    args = [a for a in sys.argv[1:] if a != '-v']
    ra, rb = open(args[0], 'rb').read(), open(args[1], 'rb').read()
    if ra == rb:
        print('IDENTICAL', len(ra))
        return 0
    a, b = SOut(ra), SOut(rb)
    sa, sb = a.segs[0], b.segs[0]
    print('file %d %d, code %d %d, data %d %d, bss %d %d' % (len(ra), len(rb), sa['code'], sb['code'], sa['data'], sb['data'], sa['bss'], sb['bss']))
    ia, ib = bytes(a.image), bytes(b.image)
    ca, cb = sa['code'], sb['code']
    d = [i for i in range(min(len(ia), len(ib))) if ia[i] != ib[i]]
    print('differing bytes %d, first code %s' % (len(d), next((hex(i) for i in range(min(ca, cb)) if ia[i] != ib[i]), None)))
    da, db = ia[ca:], ib[cb:]
    i = next((i for i in range(min(len(da), len(db))) if da[i] != db[i]), None)
    if i is not None:
        print('first data at offset %d:\n  %r\n  %r' % (i, da[max(0, i - 8):i + 40], db[max(0, i - 8):i + 40]))
    A = list(zcode.insns(ia[:ca], 0, ca, False))
    B = list(zcode.insns(ib[:cb], 0, cb, False))
    sm = difflib.SequenceMatcher(None, [norm(t) for _, t in A], [norm(t) for _, t in B], autojunk=False)
    ops = [o for o in sm.get_opcodes() if o[0] != 'equal']
    print('instruction blocks that differ: %d' % len(ops))
    for _, i, j, k, l in ops[:40]:
        print('  orig %s\n   new %s' % (['%x %s' % (x, t.replace('\t', ' ')) for x, t in A[i:j]][:6],
                                        ['%x %s' % (x, t.replace('\t', ' ')) for x, t in B[k:l]][:6]))
    if verbose and len(A) == len(B):
        c = collections.Counter()
        for (_, x), (_, y) in zip(A, B):
            for u, v in zip(re.findall(r'%([0-9A-F]{4})', x), re.findall(r'%([0-9A-F]{4})', y)):
                if u != v:
                    c[(v, u)] += 1
        print('addresses new -> orig:', c.most_common(24))
    return 1


if __name__ == '__main__':
    sys.exit(main())
