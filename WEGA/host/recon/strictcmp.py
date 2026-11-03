#!/usr/bin/env python3
"""strictcmp.py - compare every function of our linked program with the original (configuration: progcfg.py).

Usage: strictcmp.py [-m] [-c] [-a] [-v NAME] [LINKED]

  LINKED   our linked program with symbols (default progcfg STRICT_INPUT). For a strict comparison its data and bss layout
           must equal the original (for a program whose strings are laid out differently by our cc: the d12model link).
  -m       masked comparison (every hex operand of 3+ digits masked, the form of the old ldcheck/wholecmp); it hides
           wrong constants and wrong data addresses and is only a first look
  -c       call and calr count as different (without it they are equal: the strict comparison hides D17)
  -a       as -c, and @r15 and %0000(r15) count as different too (without progcfg D18): as near to the bytes as the
           comparison goes while the code addresses still differ
  -v NAME  print the diff of NAME (original | ours)

Functions and their original ranges: findings.json (functions: name -> orig, size_orig; status crt0 is skipped). Our
function is the symbol _NAME up to the next text symbol. Strict: the jumps of functions with the same lines must go to the same instruction or known function; only the targets of jr/jp/djnz/call/calr and immediates
that are function addresses (of either binary) are masked (zcode.norm_strict). Prints one line per function and the totals.
"""
import difflib
import json
import os
import re
import sys

import progcfg
import zcode

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402

JUMPCALL = re.compile(r'^(jr|jp|djnz|dbjnz|call|calr)\b')
TARGET = re.compile(r'%([0-9A-Fa-f]{3,8})\b')
TEXT_TYPES = (31, 34)   # file names and global text symbols in our s.out


def wrong_targets(gc, lo, hi, oc, s, e, orig_names, ours_names):
    """The norm masks the targets of jumps and calls: of two functions with the same lines, the jumps that go to another
    instruction of the function or to another known function (lines of the diff, one per jump)."""
    a = list(zcode.insns(gc, lo, hi, CFG.SEG))
    b = list(zcode.insns(oc, s, e, CFG.SEG))
    ia = {x: i for i, (x, _) in enumerate(a)}
    ib = {x: i for i, (x, _) in enumerate(b)}
    d = []
    for (xa, ta), (xb, tb) in zip(a, b):
        ma = TARGET.search(ta) if JUMPCALL.match(ta) else None
        mb = TARGET.search(tb) if JUMPCALL.match(tb) else None
        if not ma or not mb:
            continue
        va, vb = int(ma.group(1), 16), int(mb.group(1), 16)
        ra = ('in', ia.get(va, va - lo)) if lo <= va < hi else ('fn', orig_names.get(va))
        rb = ('in', ib.get(vb, vb - s)) if s <= vb < e else ('fn', ours_names.get(vb))
        if ra != rb:
            d.append('target %x: %s %s' % (xa, ra, rb))
    return d


def main():
    a = sys.argv[1:]
    masked = '-m' in a
    callform = '-c' in a or '-a' in a
    d18 = CFG.D18 and '-a' not in a
    a = [x for x in a if x not in ('-m', '-c', '-a')]
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
    fn = {k: f for k, f in json.load(open(CFG.FINDINGS))['functions'].items() if isinstance(f.get('orig'), str)}
    orig_fn = set(int(f['orig'], 16) for f in fn.values())
    sym = {s['name']: s['value'] for s in o.symbols}
    if masked:
        def n_ours(t): return zcode.norm_masked(t, CFG.D18)
        n_orig = n_ours
    else:
        # an immediate that is a function address of either binary is masked on both sides: a data address of one
        # side can equal a function address of the other (with the data model at the original's addresses)
        both_fn = ours_fn | orig_fn
        def n_ours(t): return zcode.norm_strict(t, both_fn, d18=d18, callform=callform)
        def n_orig(t): return zcode.norm_strict(t, both_fn, d18=d18, callform=callform)
    orig_names = {int(f['orig'], 16): k for k, f in fn.items()}
    ours_names = {x['value']: x['name'][1:] for x in o.symbols if x['type'] == 34 and x['name'][1:] in fn}
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
        if not d and not masked:
            d = wrong_targets(gc, lo, hi, oc, s, e, orig_names, ours_names)
        same += not d
        diff += bool(d)
        print('%-16s %04x %4d %4d  %-10s %s' % (name, lo, hi - lo, e - s, f.get('status'), 'SAME' if not d else 'DIFF %d' % len(d)))
        if verbose == name:
            print('\n'.join(difflib.unified_diff(theirs, mine, 'orig', 'ours', lineterm='', n=int(os.environ.get('STRICT_CTX', '2')))))
    print('same %d, differ %d (%s)' % (same, diff, 'masked' if masked else
                                       'all forms' if '-a' in sys.argv else 'call forms' if callform else 'strict'))


if __name__ == '__main__':
    main()
