#!/usr/bin/env python3
"""segcmp.py - compare every function of our linked program with the original for a program with several code segments
(progcfg CODESEGS, gopt). The counterpart of strictcmp.py, which knows one code segment.

Usage: RECON_PROG=NAME segcmp.py [-m] [-c] [-v NAME] [-V] [LINKED]

  LINKED   our linked, unstripped program (default progcfg STRICT_INPUT). Its code segments are taken from its own header
           (the segment numbers and sizes may differ from the original's: the functions are found by symbol).
  -m       masked (every hex operand of 3+ digits), -c: call and calr differ, -v NAME / -V: print the diff of NAME / of all.

findings.json: name -> orig "SEG:OFFSET", size_orig. Our function is the symbol _NAME up to the next text symbol of the same
segment (or the end of that segment). Jump and call targets are masked in both (segment number too), as in strictcmp.
Prints one line per function and the totals; `missing` when the symbol is not in our program.
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


def code_segments(s):
    """segno -> code bytes of an s.out (the image holds the code of every segment in header order, then the data)"""
    out, o = {}, 0
    for g in s.segs:
        if g['code']:
            out[g['segno']] = bytes(s.image[o:o + g['code']])
            o += g['code'] + g['data']
    return out


def orig_segments():
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    return code_segments(g)


def norm(t, both_fn, callform):
    t = re.sub(r'<<\d+>>(%[0-9A-Fa-f]{3,8})', r'\1', t) if re.match(r'^(jr|jp|jpr|djnz|dbjnz|call|calr)\b', t) else t
    return zcode.norm_strict(t, both_fn, d18=False, callform=callform)


def main():
    a = sys.argv[1:]
    masked, callform = '-m' in a, '-c' in a
    a = [x for x in a if x not in ('-m', '-c')]
    verbose = None
    if '-V' in a:
        verbose = '*'
        a.remove('-V')
    if '-v' in a:
        i = a.index('-v')
        verbose = a[i + 1]
        del a[i:i + 2]
    path = a[0] if a else CFG.STRICT_INPUT
    o = SOut(open(path, 'rb').read())
    oc, gc = code_segments(o), orig_segments()
    text = {}
    sym = {}
    for x in o.symbols:
        if x['type'] in (34, 98) and x.get('segt') in oc:
            text.setdefault(x['segt'], set()).add(x['value'] & 0xFFFF)
            sym[x['name']] = (x['segt'], x['value'] & 0xFFFF)
    fn = {k: f for k, f in json.load(open(CFG.FINDINGS))['functions'].items() if isinstance(f.get('orig'), str)}
    both = set(v[1] for v in sym.values()) | set(int(f['orig'].split(':')[1], 16) for f in fn.values())
    same = diff = 0
    for name, f in sorted(fn.items(), key=lambda x: (int(x[1]['orig'].split(':')[0]), int(x[1]['orig'].split(':')[1], 16))):
        if f.get('status') == 'crt0':
            continue
        osg, olo = f['orig'].split(':')
        osg, olo = int(osg), int(olo, 16)
        ohi = olo + f['size_orig']
        if '_' + name not in sym:
            print('%-16s %s  missing in %s' % (name, f['orig'], path))
            diff += 1
            continue
        sg, s = sym['_' + name]
        e = min([t for t in text[sg] if t > s] + [len(oc[sg])])
        if masked:
            n = zcode.norm_masked
        else:
            def n(t):
                return norm(t, both, callform)
        mine = [n(t) for _, t in zcode.insns(oc[sg], s, e, True)]
        theirs = [n(t) for _, t in zcode.insns(gc[osg], olo, ohi, True)]
        d = [l for l in difflib.unified_diff(theirs, mine, lineterm='', n=0) if l[:1] in '+-' and l[:3] not in ('---', '+++')]
        same += not d
        diff += bool(d)
        print('%-16s %s %4d %4d  %-10s %s' % (name, f['orig'], ohi - olo, e - s, f.get('status'), 'SAME' if not d else 'DIFF %d' % len(d)))
        if verbose == name or (verbose == '*' and d):
            print('\n'.join(difflib.unified_diff(theirs, mine, 'orig', 'ours', lineterm='', n=int(os.environ.get('STRICT_CTX', '2')))))
    print('same %d, differ %d (%s)' % (same, diff, 'masked' if masked else 'call forms' if callform else 'strict'))


if __name__ == '__main__':
    main()
