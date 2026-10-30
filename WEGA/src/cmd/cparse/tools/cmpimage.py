#!/usr/bin/env python3
"""cmpimage.py A B - compare the segment sizes and the code and data images of two s.out programs (symbols ignored)."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', 'host', 'tools'))
from sout import SOut  # noqa: E402

a, b = (SOut(open(p, 'rb').read()) for p in sys.argv[1:3])


def img(x):
    return bytes(x.image)[:x.segs[0]['code'] + x.segs[0]['data']]


sa = [(s['code'], s['data'], s['bss']) for s in a.segs]
sb = [(s['code'], s['data'], s['bss']) for s in b.segs]
if sa == sb and img(a) == img(b):
    print('%s: equal to %s (code, data, bss sizes)' % tuple(sys.argv[1:3]))
    sys.exit(0)
print('%s differs from %s: sizes %s / %s' % (sys.argv[1], sys.argv[2], sa, sb))
sys.exit(1)
