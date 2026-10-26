#!/usr/bin/env python3
"""confirm.py - checks the pairings stored in findings.json against the built binary (one source of truth).

findings.json holds the decisions (pairings, reviews, rejects, classes, findings). This script does not decide
pairings; it checks the stored ones:
  - every function is located by name (locate.py), so our addresses are never stored as truth;
  - a stored pairing (our name -> original start) gets the evidence it has: the same instructions (strict-1),
    the same formatted strings, or the review evidence stored in findings.json (key "manual", confirm true);
  - a pairing without any evidence is reported and marked "open", not silently kept.
Writes functions[name].ours (generated), pairing, pairing_evidence and level.

Usage: python3 confirm.py --mine ./codgen --orig $WEGA/lib/codgen [--seg]
"""
import argparse
import collections
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import funcs as F  # noqa: E402
import locate as L  # noqa: E402
from sout import SOut  # noqa: E402

LIBC_FROM = 0xbbc4   # start of the C library in /lib/codgen (host/KNOWLEDGE.md)
NEEDLE = b'freeing blank tree!'   # the string of free1 (comm3.c), the same in both binaries


def string_offset(rows, data, start):
    """File offset minus address of the data (constant per binary), found through the string of free1."""
    pos = data.find(NEEDLE)
    for raw in F.extent(rows, start, start + 0x40):
        m = re.match(r'^ld\s+r\d+,#%([0-9A-F]{4})$', raw)
        if m and pos >= 0:
            return pos - int(m.group(1), 16)
    raise SystemExit('string offset not found (free1 at 0x%04x)' % start)


def strings(rows, data, off, start, end):
    """The printable strings (>= 2 characters) that the function loads with ld rN,#%XXXX."""
    out = set()
    for raw in F.extent(rows, start, end):
        m = re.match(r'^ld\s+r\d+,#%([0-9A-F]{4})$', raw)
        if not m:
            continue
        x = int(m.group(1), 16) + off
        if x < 0 or x >= len(data):
            continue
        t = data[x:x + 200].split(b'\0')[0]
        if len(t) >= 2 and all(32 <= c < 127 or c in (9, 10) for c in t):
            out.add(t)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--mine', default='./codgen')
    ap.add_argument('--orig', default=os.path.expandvars('$WEGA/lib/codgen'))
    ap.add_argument('--findings', default=os.path.join(HERE, 'findings.json'))
    ap.add_argument('--seg', action='store_true', help='scodgen build')
    a = ap.parse_args()

    fj = json.load(open(a.findings))
    f = fj['functions']
    mine_data = open(a.mine, 'rb').read()
    s = SOut(open(a.mine, 'rb').read())
    lim = F.own_limit(s)
    mine_rows = F.disasm(a.mine, lim)
    orig_rows = F.disasm(a.orig)
    orig_data = open(a.orig, 'rb').read()
    _, cand = F.orig_candidates(orig_rows)
    cand = sorted(set(c for c in cand if c < LIBC_FROM))

    def orig_end(o):
        n = [c for c in cand if c > o]
        return n[0] if n else o + 0x200

    located, problems, _ = L.locate(a.mine, os.path.join(HERE, '..'), a.seg)
    for p in problems:
        print('PROBLEM', p)
    ours = {}   # name -> (start, end): end is the next function start in the binary (objects are contiguous)
    allstarts = sorted(e['addr'] for lst in located.values() for e in lst)
    for fn, lst in located.items():
        for e in lst:
            nxt = [x for x in allstarts if x > e['addr']]
            ours[e['name']] = (e['addr'], nxt[0] if nxt else lim)
    off_m = string_offset(mine_rows, mine_data, ours['free1'][0])
    off_o = string_offset(orig_rows, orig_data, 0x8758)

    counts = collections.Counter()
    for name, v in f.items():
        if name not in ours:
            v['ours'] = None
            v['pairing'] = v.get('pairing', 'none') if v.get('pairing') == 'none' else 'not located'
            counts['not located'] += 1
            continue
        st, en = ours[name]
        v['ours'] = '0x%04x-0x%04x' % (st, en)
        if v.get('pairing') == 'none' or not v.get('orig'):
            counts['none'] += 1
            continue
        g = int(v['orig'], 16)
        level, _ = F.strict_compare(F.extent(mine_rows, st, en), F.extent(orig_rows, g, orig_end(g)))
        strs = strings(mine_rows, mine_data, off_m, st, en)
        same_str = strs == strings(orig_rows, orig_data, off_o, g, orig_end(g))
        man = fj.get('manual', {}).get(name, {})
        if name in fj.get('reject', {}):
            ev = None
        elif level in ('exact', 'order'):
            ev = 'same instructions (%s)' % level
        elif same_str and strs:
            ev = 'same strings: %d' % len(strs)
        elif man.get('confirm') and man.get('orig') and int(man['orig'], 16) == g:
            ev = man.get('evidence', 'review')
        else:
            ev = None
        if ev:
            v['pairing'] = 'confirmed'
            v['pairing_evidence'] = ev
            counts['confirmed'] += 1
        else:
            v['pairing'] = 'open'
            v['pairing_evidence'] = 'no evidence in the instructions, the strings or a review'
            counts['open'] += 1
        v['level'] = level
    json.dump(fj, open(a.findings, 'w'), indent=1)
    print(dict(counts))
    dups = [k for k, c in collections.Counter(v.get('orig') for v in f.values() if v.get('pairing') == 'confirmed').items() if c > 1]
    if dups:
        print('DUPLICATE original functions:', dups)


if __name__ == '__main__':
    main()
