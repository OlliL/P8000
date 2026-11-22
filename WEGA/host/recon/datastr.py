#!/usr/bin/env python3
"""datastr.py - compare the data segment of our linked program with the original by its strings (gopt).

Usage: RECON_PROG=NAME datastr.py [LINKED]            the strings that differ (difflib over the C strings in order)
       RECON_PROG=NAME datastr.py -s [LINKED]         the shifts: for every string of the original that ours has too, the
                                                      offset difference where it changes (finds the place of a missing or extra byte)
       RECON_PROG=NAME datastr.py -b LO HI [SHIFT] [LINKED]   the bytes LO..HI (hex) of the original and of ours (at LO+SHIFT)

Pointers and switch tables show up as odd short strings: ignore the one-character noise, look at the real strings and the shifts.
"""
import difflib
import os
import re
import sys

import progcfg

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402


def load(path):
    o = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    n = SOut(open(path, 'rb').read())
    return (bytes(o.image)[sum(x[1] for x in CFG.CODESEGS.values()):], bytes(n.image)[sum(s['code'] for s in n.segs):])


def strs(d, n=1):
    return [(m.start(), m.group()[:-1]) for m in re.finditer(rb'[\x20-\x7e\t\n]{%d,}\x00' % n, d)]


def main():
    a = sys.argv[1:]
    shifts = '-s' in a
    byt = '-b' in a
    a = [x for x in a if x not in ('-s', '-b')]
    default = os.path.join(CFG.REPO, 'src', 'cmd', 'cc', CFG.NAME if hasattr(CFG, 'NAME') else 'gopt', 'gopt')
    if byt:
        lo, hi = int(a[0], 16), int(a[1], 16)
        sh = int(a[2]) if len(a) > 2 and not a[2].endswith('opt') else 0
        do, dn = load(default)
        print(do[lo:hi])
        print(dn[lo + sh:hi + sh])
        return
    do, dn = load(a[0] if a else default)
    print('data: original %d, ours %d' % (len(do), len(dn)))
    if shifts:
        ao = dict((s, p) for p, s in strs(do, 4))
        bo = dict((s, p) for p, s in strs(dn, 4))
        last = None
        for s, p in sorted(ao.items(), key=lambda x: x[1]):
            if s in bo and bo[s] - p != last:
                last = bo[s] - p
                print('%04x %+d %r' % (p, last, s[:40]))
        return
    A, B = strs(do), strs(dn)
    sm = difflib.SequenceMatcher(None, [x[1] for x in A], [x[1] for x in B], autojunk=False)
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag != 'equal':
            print(tag, '%04x' % A[i1][0] if i1 < len(A) else '', [x[1][:60] for x in A[i1:i2]], '|', [x[1][:60] for x in B[j1:j2]])


if __name__ == '__main__':
    main()
