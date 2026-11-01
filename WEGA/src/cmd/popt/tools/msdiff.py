#!/usr/bin/env python3
"""msdiff.py NAME NEXT: the instructions that differ between the original function NAME and ours as multisets
(the - and + lines of `strictcmp.py -v NAME` up to the line of the next function NEXT, counted without their
order): what is content and not layout. Run in src/cmd/popt after `oldcc.sh model popt`."""
import subprocess
import sys
from collections import Counter

f, nxt = sys.argv[1], sys.argv[2]
out = subprocess.run(['python3', 'tools/strictcmp.py', '-v', f], capture_output=True, text=True).stdout
d = out[out.index('--- orig'):out.index('\n' + nxt)]
minus = Counter(l[1:] for l in d.split('\n') if l.startswith('-') and not l.startswith('---'))
plus = Counter(l[1:] for l in d.split('\n') if l.startswith('+') and not l.startswith('+++'))
for title, c in (('only orig', minus - plus), ('only ours', plus - minus)):
    items = sorted(c.items(), key=lambda x: -x[1])
    print('%s: %d' % (title, sum(c.values())))
    for k, v in items:
        print('  %3d  %s' % (v, k))
