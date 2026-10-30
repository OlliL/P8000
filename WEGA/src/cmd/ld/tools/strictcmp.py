#!/usr/bin/env python3
"""strictcmp.py - strict comparison of every own function of ld with /bin/ld (no masking of data addresses or constants).

Usage: strictcmp.py [LDM] [-v NAME]

  LDM      a link of the rebuild whose data and bss layout equals the original: the output of d12model.py
           (default /mnt/share/cvs/P8000/tmp/tmp/d12model/ldm, run d12model.py first)
  -v NAME  print the diff of NAME

wholecmp.py masks every hex operand of 3+ digits, so a wrong constant or a wrong data/bss address passes there. Here
only code addresses are masked: the targets of jr/jp/djnz/call/calr, and an operand that is the address of a text symbol
in LDM (a function pointer). Everything else (immediates, data and bss addresses, frame offsets) must be equal. calr
and call are equal (D17). The data layout of LDM equals the original only under the D12 model: check the d12model.py
report first (data size equal, only code addresses in tables differ).
"""
import difflib
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import ldcheck  # noqa: E402
from sout import SOut  # noqa: E402

JUMP = re.compile(r'^(jr|jp|djnz|dbjnz|call|calr)\b')


def norm(t, textaddr):
    t = re.sub(r'^calr\t', 'call\t', t)
    if JUMP.match(t):
        return re.sub(r'%[0-9A-Fa-f]{3,4}\b', 'CODE', t)
    return re.sub(r'#%([0-9A-Fa-f]{3,4})\b', lambda m: '#CODE' if int(m.group(1), 16) in textaddr and int(m.group(1), 16) >= 0x42 else m.group(0), t)


def main():
    a = sys.argv[1:]
    verbose = None
    if '-v' in a:
        i = a.index('-v')
        verbose = a[i + 1]
        del a[i:i + 2]
    path = a[0] if a else '/mnt/share/cvs/P8000/tmp/tmp/d12model/ldm'
    o = SOut(open(path, 'rb').read())
    oc = bytes(o.image)[:o.segs[0]['code']]
    g = SOut(open(ldcheck.ORIG, 'rb').read())
    gc = bytes(g.image)[:g.segs[0]['code']]
    text = sorted(set(s['value'] for s in o.symbols if s['type'] in (31, 34) and s['value'] < len(oc)))
    textaddr = set(s['value'] for s in o.symbols if s['type'] == 34)
    fn = json.load(open(os.path.join(HERE, 'findings.json')))['functions']
    otext = set(int(f['orig'], 16) for f in fn.values())
    sym = {s['name']: s['value'] for s in o.symbols}
    same = diff = 0
    for name, f in sorted(fn.items(), key=lambda x: int(x[1]['orig'], 16)):
        if f.get('status') == 'crt0':
            continue
        lo = int(f['orig'], 16)
        hi = lo + f['size_orig']
        s = sym.get('_' + name)
        if s is None:
            print('%-16s %04x  missing' % (name, lo))
            continue
        e = min([t for t in text if t > s] + [len(oc)])
        mine = [norm(t, textaddr) for _, t in ldcheck.insns(oc, s, e)]
        theirs = [norm(t, otext) for _, t in ldcheck.insns(gc, lo, hi)]
        d = [l for l in difflib.unified_diff(theirs, mine, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++')]
        same += not d
        diff += bool(d)
        print('%-16s %04x %4d %4d  %-10s %s' % (name, lo, hi - lo, e - s, f.get('status'), 'SAME' if not d else 'DIFF %d' % len(d)))
        if verbose == name:
            print('\n'.join(difflib.unified_diff(theirs, mine, 'orig', 'ours', lineterm='', n=2)))
    print('same %d, differ %d' % (same, diff))


if __name__ == '__main__':
    main()
