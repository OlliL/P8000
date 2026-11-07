#!/usr/bin/env python3
"""own_table.py INVENTORY.json - the first findings.json of a program (configuration: progcfg.py).

Reads the inventory JSON (inventory.py) and enters every function start of the own code (0 .. progcfg OWN_LIM; the libc
follows, see host/tools/whichlibc.py) in findings.json of the tools directory: name f<entry> until it is named (rename.py),
orig, size_orig (to the next start), status open, kind (proven: a call target; candidate: a start after ret or a jump),
callers. Functions already in findings.json (by orig) are kept as they are.
"""
import json
import os
import sys

import progcfg

CFG = progcfg.load()
LIBC = CFG.OWN_LIM
OUT = CFG.FINDINGS
fj = json.load(open(OUT)) if os.path.exists(OUT) else {
    'schema': 'functions: name -> file, orig, size_orig, status (open, exact), kind, callers, note (by hand: an odd source form, '
              'a guessed name, dead code).',
    'compiler': '?', 'binary': '/' + CFG.ORIG_BIN, 'functions': {}}
have = {int(f['orig'], 16) for f in fj['functions'].values()}
src = sys.argv[1]
d = json.load(open(src))
starts = sorted(set(int(x, 16) for x in d['proven_starts']) | set(int(x, 16) for x in d['candidate_starts_after_ret_or_jump']))
own = [s for s in starts if s < LIBC]
callers = {}
for f in d['functions']:
    for c in f['calls']:
        callers.setdefault(int(c, 16), []).append(int(f['start'], 16))
proven = set(int(x, 16) for x in d['proven_starts'])
new = 0
for i, s in enumerate(own):
    if s in have:
        continue
    e = own[i + 1] if i + 1 < len(own) else LIBC
    fj['functions']['f%04x' % s] = {'file': '', 'orig': '0x%04x' % s, 'size_orig': e - s, 'status': 'open',
                                   'kind': 'proven' if s in proven else 'candidate',
                                   'callers': ['0x%04x' % c for c in sorted(set(callers.get(s, [])))]}
    new += 1
json.dump(fj, open(OUT, 'w'), indent=1)
open(OUT, 'a').write('\n')
print(len(own), 'own functions,', sum(1 for s in own if s in proven), 'proven,', new, 'new in', OUT)
