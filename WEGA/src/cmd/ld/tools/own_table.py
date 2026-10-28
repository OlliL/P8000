#!/usr/bin/env python3
"""own_table.py - function table of the own code of /bin/ld (nonsegmented, stripped).

Reads the inventory JSON (inventory.py) and writes own_functions.txt: entry, end (next start), size, callers,
kind, status. Own code = 0x0000 .. LIBC (the shipped libc members start at 0x2c4c, see libcver.py).
"""
import json
import sys

LIBC = 0x2c4c
src = sys.argv[1]
d = json.load(open(src))
starts = sorted(set(int(x, 16) for x in d['proven_starts']) | set(int(x, 16) for x in d['candidate_starts_after_ret_or_jump']))
own = [s for s in starts if s < LIBC]
callers = {}
for f in d['functions']:
    for c in f['calls']:
        callers.setdefault(int(c, 16), []).append(int(f['start'], 16))
proven = set(int(x, 16) for x in d['proven_starts'])
rows = ['# entry  name  end    size  callers  kind      status',
        '# name: f<entry> until it is understood (names in the 2nd column); kind: proven (call target) or candidate',
        '# status: open (no C draft yet)']
for i, s in enumerate(own):
    e = own[i + 1] if i + 1 < len(own) else LIBC
    cl = ','.join('%04x' % c for c in sorted(set(callers.get(s, []))))
    kind = 'proven' if s in proven else 'candidate'
    rows.append('%04x  f%04x  %04x  %5d  %-16s %-9s open' % (s, s, e, e - s, cl or '-', kind))
open('own_functions.txt', 'w').write('\n'.join(rows) + '\n')
print(len(own), 'own functions,', sum(1 for s in own if s in proven), 'proven')
