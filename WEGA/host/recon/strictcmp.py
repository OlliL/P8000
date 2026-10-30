#!/usr/bin/env python3
"""strictcmp.py - compare every function of our linked program with the original (configuration: progcfg.py).

Usage: strictcmp.py [-m] [-v NAME] [LINKED]

  LINKED   our linked program with symbols (default progcfg STRICT_INPUT). For a strict comparison its data and bss layout
           must equal the original (for a program whose strings are laid out differently by our cc: the d12model link).
  -m       masked comparison (every hex operand of 3+ digits masked, the form of the old ldcheck/wholecmp); it hides
           wrong constants and wrong data addresses and is only a first look
  -v NAME  print the diff of NAME (original | ours)

Functions and their original ranges: findings.json (functions: name -> orig, size_orig; status crt0 is skipped). Our
function is the symbol _NAME up to the next text symbol. Strict: only the targets of jr/jp/djnz/call/calr and immediates
that are function addresses are masked (zcode.norm_strict). Prints one line per function and the totals.
"""
import difflib
import json
import os
import sys

import progcfg
import zcode

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402

TEXT_TYPES = (31, 34)   # file names and global text symbols in our s.out


def main():
    a = sys.argv[1:]
    masked = '-m' in a
    a = [x for x in a if x != '-m']
    verbose = None
    if '-v' in a:
        i = a.index('-v')
        verbose = a[i + 1]
        del a[i:i + 2]
    path = a[0] if a else CFG.STRICT_INPUT
    o = SOut(open(path, 'rb').read())
    oc = bytes(o.image)[:o.segs[0]['code']]
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    gc = bytes(g.image)[:g.segs[0]['code']]
    text = sorted(set(s['value'] for s in o.symbols if s['type'] in TEXT_TYPES and s['value'] < len(oc)))
    ours_fn = set(s['value'] for s in o.symbols if s['type'] == 34)
    fn = json.load(open(CFG.FINDINGS))['functions']
    orig_fn = set(int(f['orig'], 16) for f in fn.values())
    sym = {s['name']: s['value'] for s in o.symbols}
    if masked:
        n_ours = n_orig = zcode.norm_masked
    else:
        def n_ours(t): return zcode.norm_strict(t, ours_fn)
        def n_orig(t): return zcode.norm_strict(t, orig_fn)
    same = diff = 0
    for name, f in sorted(fn.items(), key=lambda x: int(x[1]['orig'], 16)):
        if f.get('status') == 'crt0':
            continue
        lo = int(f['orig'], 16)
        hi = lo + f['size_orig']
        s = sym.get('_' + name)
        if s is None:
            print('%-16s %04x  missing in %s' % (name, lo, path))
            diff += 1
            continue
        e = min([t for t in text if t > s] + [len(oc)])
        mine = [n_ours(t) for _, t in zcode.insns(oc, s, e, CFG.SEG)]
        theirs = [n_orig(t) for _, t in zcode.insns(gc, lo, hi, CFG.SEG)]
        d = [l for l in difflib.unified_diff(theirs, mine, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++')]
        same += not d
        diff += bool(d)
        print('%-16s %04x %4d %4d  %-10s %s' % (name, lo, hi - lo, e - s, f.get('status'), 'SAME' if not d else 'DIFF %d' % len(d)))
        if verbose == name:
            print('\n'.join(difflib.unified_diff(theirs, mine, 'orig', 'ours', lineterm='', n=2)))
    print('same %d, differ %d (%s)' % (same, diff, 'masked' if masked else 'strict'))


if __name__ == '__main__':
    main()
