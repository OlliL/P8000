#!/usr/bin/env python3
"""framemap.py - the functions of a linked segmented program (code segments 4 and 5) that differ from the original, when both
have the same layout (the same addresses: the masked compare of fnc.py is equal, the link is byte-exact but for some bytes).

Usage: framemap.py ORIG NEW_STRIPPED NEW_UNSTRIPPED [FUNC] [-o]

The functions are taken from the symbols of NEW_UNSTRIPPED (a static function counts to the one in front of it). For each
function that differs, the instructions of ORIG and NEW are paired one by one; a pair that differs only in a frame offset
(|<<63>>%N|(r15), <<63>>%N(r15), the address #%BF00N of a local) gives the mapping new -> orig of that offset:

  NAME  SEG:OFF n=INSTRUCTIONS other=PAIRS_THAT_DIFFER_OTHERWISE bad=INCONSISTENT  NEW>ORIG ...

The locals of a function lie in the frame in the order of their declaration, behind the saved parameters (the first one
declared has the lowest offset); a mapping that only mirrors when two declarations are swapped is a chain or a statement
order (q = last = x against last = q = x). FUNC prints its differing pairs; -o prints the other pairs of all functions
(constants, globals, members, jump targets). Used for /lib/gopt (compare/gopt/HANDOFF.md, step 1).
"""
import collections
import os
import re
import sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tools'))
from sout import SOut
import unlink_seg as U
import z8k

o = SOut(open(sys.argv[1], 'rb').read()); n = SOut(open(sys.argv[2], 'rb').read()); g = SOut(open(sys.argv[3], 'rb').read())
only = sys.argv[4] if len(sys.argv) > 4 and sys.argv[4] != '-o' else None
oi, ni = bytes(o.image), bytes(n.image)
segs = {4: (0, 65534), 5: (65534, 50432)}
syms = collections.defaultdict(list)
for x in g.symbols:
    if x['segt'] in (4, 5) and x['name'].startswith('_'):
        syms[x['segt']].append((x['value'] & 0xffff, x['name'][1:]))
OFF = re.compile(r'\|<<63>>%([0-9A-F]{4})\|\(r15\)|<<63>>%([0-9A-F]{4})\(r15\)|#%BF00([0-9A-F]{4})')
for sg, (base, size) in segs.items():
    lst = sorted(set(syms[sg]))
    for k, (lo, name) in enumerate(lst):
        hi = lst[k + 1][0] if k + 1 < len(lst) else size
        if only and name != only:
            continue
        try:
            A = [z8k.text(i) for pc, i in U.decode_range(oi[base:base + size], sg, lo, hi)]
            B = [z8k.text(i) for pc, i in U.decode_range(ni[base:base + size], sg, lo, hi)]
        except ValueError:
            continue
        if A == B:
            continue
        m = collections.OrderedDict(); bad = 0; other = 0
        for a, b in zip(A, B):
            if a == b:
                continue
            if OFF.sub('X', a) != OFF.sub('X', b):
                other += 1
                if '-o' in sys.argv:
                    print('   %s: orig %-36s ours %s' % (name, a, b))
                continue
            for x, y in zip(OFF.finditer(b), OFF.finditer(a)):
                vb = int([v for v in x.groups() if v][0], 16); va = int([v for v in y.groups() if v][0], 16)
                if m.get(vb, va) != va:
                    bad += 1
                m[vb] = va
        print('%-16s %d:%04x n=%d other=%d bad=%d  %s' % (name, sg, lo, len(A), other, bad,
              ' '.join('%x>%x' % (k2, v2) for k2, v2 in sorted(m.items()) if k2 != v2)))
        if only:
            for a, b in zip(A, B):
                if a != b:
                    print('   orig %-40s ours %s' % (a, b))
