#!/usr/bin/env python3
"""headcomments.py - write the head comment of every own function of the C files: `/* name: description (0xA..0xB of /bin/sld) */`.
The range comes from own_functions.txt, the description from DESC below. An old one-line status comment (`/* name..xxxx: done */`)
is replaced; any other comment in front of the function stays. Run again after a rename (rename.py changes the names in DESC? no:
edit DESC)."""
import glob
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
DESC = {}
for l in open(os.path.join(HERE, 'headcomments.txt')):
    if l.strip() and not l.startswith('#'):
        n, d = l.rstrip('\n').split(None, 1)
        DESC[n] = d
tab = {}
for l in open(os.path.join(HERE, 'own_functions.txt')):
    p = l.split()
    if len(p) > 3 and not l.startswith('#'):
        tab[p[1]] = (p[0], p[2])
DEF = re.compile(r'^(?:int |long |char |unsigned |struct \w+ \*?)?\**([a-z_0-9]+)\(.*\)$')
OLD = re.compile(r'^/\* (\w+)(\.\.[0-9a-f]+| 0x[0-9a-f]+\.\.0x[0-9a-f]+)?: (done|findings[^*]*) \*/$|^/\* f[0-9a-f]+\.\.[0-9a-f]+: .*\*/$|^/\* \w+ \(0x[0-9a-f]+\.\.0x[0-9a-f]+ of /bin/sld\).*\*/$|^/\* \w+: .* \(0x[0-9a-f]+\.\.0x[0-9a-f]+ of /bin/sld\) \*/$')
for f in sorted(glob.glob(os.path.join(HERE, '..', '*.c'))):
    s = open(f).read().split('\n')
    out = []
    for i, l in enumerate(s):
        m = DEF.match(l)
        if m and m.group(1) in tab:
            n = m.group(1)
            while out and OLD.match(out[-1]):
                out.pop()
            lo, hi = tab[n]
            out.append('/* %s: %s (0x%s..0x%s of /bin/sld) */' % (n, DESC[n], lo, hi))
        out.append(l)
    open(f, 'w').write('\n'.join(out))
