#!/usr/bin/env python3
"""bsspair.py - the bss variables of /lib/cparse: original address -> our name, found by pairing the address operands of
the original and of our d12model link (progcfg MODEL_OUT) in the equal stretches of each function (masked compare).

Usage: bsspair.py [-a]      prints: original address, size up to the next one, our name, our address, votes
         -a  also the data addresses (below the bss)
"""
import collections
import difflib
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon'))
import progcfg  # noqa: E402
import zcode  # noqa: E402

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402

ADDR = re.compile(r'%([0-9A-F]{4})\b')


def main():
    alld = '-a' in sys.argv
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    o = SOut(open(CFG.MODEL_OUT, 'rb').read())
    gc = bytes(g.image)[:g.segs[0]['code']]
    oc = bytes(o.image)[:o.segs[0]['code']]
    gbss = g.segs[0]['data']
    obss = o.segs[0]['data']
    osym = {}
    for s in o.symbols:
        if s['type'] in (3, 4, 35, 36):
            osym.setdefault(s['value'], s['name'])
    otext = sorted((s['value'], s['name'][1:]) for s in o.symbols if s['type'] in (2, 34) and s['name'].startswith('_'))
    ends = [v for v, n in otext] + [len(oc)]
    fn = json.load(open(CFG.FINDINGS))['functions']
    votes = collections.defaultdict(collections.Counter)
    for k, (v, name) in enumerate(otext):
        f = fn.get(name)
        if not f:
            continue
        lo = int(f['orig'], 16)
        a = [t for _, t in zcode.insns(gc, lo, lo + f['size_orig'], CFG.SEG)]
        b = [t for _, t in zcode.insns(oc, v, ends[k + 1], CFG.SEG)]
        ma = [zcode.norm_masked(t, CFG.D18) for t in a]
        mb = [zcode.norm_masked(t, CFG.D18) for t in b]
        sm = difflib.SequenceMatcher(None, ma, mb, autojunk=False)
        for op, i1, i2, j1, j2 in sm.get_opcodes():
            if op != 'equal':
                continue
            for i, j in zip(range(i1, i2), range(j1, j2)):
                if a[i].split('\t')[0] in ('jr', 'jp', 'call', 'calr', 'djnz') or a[i].startswith('.word'):
                    continue
                xa = [int(x, 16) for x in ADDR.findall(a[i])]
                xb = [int(x, 16) for x in ADDR.findall(b[j])]
                if len(xa) != len(xb):
                    continue
                for p, q in zip(xa, xb):
                    if (p >= gbss or alld) and q in osym:
                        votes[p][(osym[q], q)] += 1
    addrs = sorted(votes)
    for i, p in enumerate(addrs):
        (name, q), n = votes[p].most_common(1)[0]
        nxt = addrs[i + 1] if i + 1 < len(addrs) else None
        other = '' if len(votes[p]) == 1 else '  (also %s)' % ', '.join('%s' % x[0] for x, c in votes[p].most_common()[1:3])
        print('%04x %5s  %-14s %04x  %d%s' % (p, nxt - p if nxt else '-', name, q, n, other))


if __name__ == '__main__':
    main()
