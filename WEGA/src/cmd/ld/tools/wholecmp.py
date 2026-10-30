#!/usr/bin/env python3
"""wholecmp.py - compare every own function of the linked rebuild of ld with /bin/ld (whole-program check).

Usage: wholecmp.py [LDBIN] [-v NAME]

  LDBIN    the linked rebuild with symbols (default ../ld, made by `make ld`)
  -v NAME  print the instruction diff of NAME (original | ours)

Functions and original ranges come from findings.json (functions: name -> orig, size_orig). The function in LDBIN is
the symbol _NAME up to the next text symbol. Instructions are normalised with ldcheck.norm (addresses masked, frame
offsets kept, calr = call). Prints one line per function: SAME or DIFF with the sizes and the number of differing lines.
"""
import difflib
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import ldcheck  # noqa: E402
from sout import SOut  # noqa: E402


def main():
    a = sys.argv[1:]
    verbose = None
    if '-v' in a:
        i = a.index('-v')
        verbose = a[i + 1]
        del a[i:i + 2]
    path = a[0] if a else os.path.join(HERE, '..', 'ld')
    o = SOut(open(path, 'rb').read())
    oc = bytes(o.image)[:o.segs[0]['code']]
    g = SOut(open(ldcheck.ORIG, 'rb').read())
    gc = bytes(g.image)[:g.segs[0]['code']]
    text = sorted(set(s['value'] for s in o.symbols if s['type'] in (31, 34) and s['value'] < len(oc)))
    sym = {s['name']: s['value'] for s in o.symbols}
    fn = json.load(open(os.path.join(HERE, 'findings.json')))['functions']
    same = diff = 0
    for name, f in sorted(fn.items(), key=lambda x: int(x[1]['orig'], 16)):
        lo = int(f['orig'], 16)
        hi = lo + f['size_orig']
        if f.get('status') == 'crt0':
            continue
        s = sym.get('_' + name)
        if s is None:
            print('%-16s %04x  missing in %s' % (name, lo, path))
            continue
        e = min([t for t in text if t > s] + [len(oc)])
        mine = [ldcheck.norm(t) for _, t in ldcheck.insns(oc, s, e)]
        theirs = [ldcheck.norm(t) for _, t in ldcheck.insns(gc, lo, hi)]
        d = [l for l in difflib.unified_diff(theirs, mine, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++')]
        if not d:
            same += 1
        else:
            diff += 1
        print('%-16s %04x %4d %4d  %-10s %s' % (name, lo, hi - lo, e - s, f.get('status'), 'SAME' if not d else 'DIFF %d' % len(d)))
        if verbose == name:
            print('\n'.join(difflib.unified_diff(theirs, mine, 'orig', 'ours', lineterm='', n=2)))
    print('same %d, differ %d' % (same, diff))


if __name__ == '__main__':
    main()
