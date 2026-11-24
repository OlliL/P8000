#!/usr/bin/env python3
"""tccscan.py [ROOT]: the binaries of the WEGA root (bin, usr/bin, etc, lib, usr/lib) with a compare evaluated as a value
into a frame word and tested at once: `tcc cc,rN; ld F(r15),rN; test rN|F(r15); jr/jp ne|eq`. This is the form of the
libPW regcmp in /bin/regcmp (`||`/`&&` of a condition through a temporary of the compiler, increment 161, regcmp); no
compiler of the tree makes it. Prints the file, the number of places and the first addresses."""
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools'))
import libcver as L  # noqa: E402

ROOT = sys.argv[1] if len(sys.argv) > 1 else os.environ.get('WEGA_ROOT', '/home/olivleh1/git/P8000/tmp/wega31_root')
for d in ('bin', 'usr/bin', 'etc', 'lib', 'usr/lib'):
    dd = os.path.join(ROOT, d)
    if not os.path.isdir(dd):
        continue
    for f in sorted(os.listdir(dd)):
        p = os.path.join(dd, f)
        if not os.path.isfile(p) or os.path.getsize(p) > 400000:
            continue
        try:
            s, po, ro = L.load_program(p)
        except Exception:
            continue
        hits = []
        for i in range(len(po) - 4):
            t = po[i][1]
            if not t.startswith('tcc'):
                continue
            r = t.split(',')[1].strip()
            if not re.match(r'ld (%[0-9A-F]{4}\(r15\)|@r15),' + r + '$', po[i + 1][1]):
                continue
            seq = [po[i + k][1] for k in range(2, 5)]
            if any(x.startswith('test') for x in seq) and any(re.match(r'j[pr] (ne|eq),', x) for x in seq):
                hits.append(hex(po[i][0]))
        if hits:
            print('%-24s %3d  %s' % (d + '/' + f, len(hits), ' '.join(hits[:6])))
