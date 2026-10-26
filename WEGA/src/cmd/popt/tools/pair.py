#!/usr/bin/env python3
"""pair.py - ranks the candidate starts of the original /lib/popt for every function of findings.json.

For each function with an "ours" range the instruction sequence of our symbol build is compared (canon(), the
strict form of funcs.py) with the sequence of every candidate start of the original (funcs.orig_candidates).
Output: the three best candidates per function with the similarity (difflib ratio). The result is written to
the key "candidates" of each function, so the pairing can be confirmed by hand; "pairing" and "orig" are not
touched here (a pairing is only "confirmed" by the owner's review, as in codgen).

Usage: python3 pair.py --mine <popt_sym> --orig $WEGA/lib/popt [--write]
"""
import argparse
import difflib
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import funcs as F  # noqa: E402
from sout import SOut  # noqa: E402

ap = argparse.ArgumentParser()
ap.add_argument('--mine', required=True)
ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/popt'))
ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
ap.add_argument('--write', action='store_true')
a = ap.parse_args()

m = F.disasm(a.mine, F.own_limit(SOut(open(a.mine, 'rb').read())))
o = F.disasm(a.orig)
strong, _ = F.orig_candidates(o)
fj = json.load(open(a.findings))
for name, v in fj['functions'].items():
    if not v.get('ours'):
        continue
    st, en = [int(x, 16) for x in v['ours'].split('-')]
    mine = [F.canon(x) for x in F.extent(m, st, en)]
    scored = []
    for c in strong:
        cand = [F.canon(x) for x in F.extent(o, c, next((s for s in strong if s > c), 0x3838))]
        r = difflib.SequenceMatcher(None, mine, cand, autojunk=False).ratio()
        scored.append((round(r * 100, 1), '0x%04x' % c))
    scored.sort(reverse=True)
    v['candidates'] = [{'orig': s, 'similarity_pct': p} for p, s in scored[:3]]
    print('%-14s ours %s-%s  %s' % (name, v['ours'].split('-')[0], v['ours'].split('-')[1],
                                   '  '.join('%s %.1f%%' % (s, p) for p, s in scored[:3])))
if a.write:
    json.dump(fj, open(a.findings, 'w'), indent=1)
    print('candidates written to', a.findings)
