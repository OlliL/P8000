#!/usr/bin/env python3
"""dataname.py - the data address of every global of a program without symbols (gopt), from the code.

Usage: RECON_PROG=NAME dataname.py OBJ.o ... [-w FILE]     (the objects of the reconstructed modules)

For every function of findings.json (file = the module of the object) (the equal blocks of the masked code are aligned), the external
relocations of the object (symbol, addend) are matched with the data address (<<7>>%XXXX, <<8>>%XXXX, #%870 0XXXX) in the same
instruction of the original: address of the symbol = original address - addend. Prints `name seg addr` per symbol, the conflicts
and the symbols without an address; -w writes `name seg addr` lines to FILE (the table of the data definitions).
"""
import bisect
import json
import os
import re
import sys

import progcfg

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sout import SOut  # noqa: E402
import zcode  # noqa: E402
import fnc  # noqa: E402

FN = fnc.FN
ADDR = [re.compile(r'<<([78])>>%([0-9A-Fa-f]{4})'), re.compile(r'#%8([78])00([0-9A-Fa-f]{4})')]
DROP = re.compile(r'^and\sr0,#ADDR$')


def nm(t):
    return zcode.norm_masked(re.sub(r'<<\d+>>', '', t))


def main():
    a = sys.argv[1:]
    wf = None
    if '-w' in a:
        i = a.index('-w')
        wf = a[i + 1]
        del a[i:i + 2]
    g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    gimg = bytes(g.image)
    res, conf, none, cnt = {}, [], set(), {}
    for path in a:
        base = os.path.basename(path)[:-2] + '.c'
        o = SOut(open(path, 'rb').read())
        oc = bytes(o.image)[:o.segs[0]['code']]
        rel = o.reloc
        starts = sorted(s['value'] for s in o.symbols if s['type'] == 34)
        syms = {s['name']: s['value'] for s in o.symbols if s['type'] == 34}
        names = [k for k, f in FN.items() if f.get('file') == base and not re.match(r'(case block|tail) of ', f.get('note', ''))
                 and not re.match(r'f\d_', k)]
        for k in names:
            if '_' + k not in syms:
                continue
            s = syms['_' + k]
            e = min([t for t in starts if t > s] + [len(oc)])
            k0, k1 = fnc.span(k)
            sg, lo = FN[k0]['orig'].split(':')
            sg, lo = int(sg), int(lo, 16)
            hi = int(FN[k1]['orig'].split(':')[1], 16) + FN[k1]['size_orig']
            off, n = CFG.CODESEGS[sg]
            gcode = gimg[off:off + n]
            ol = [(pc, t, nm(t)) for pc, t in zcode.insns(oc, s, e, CFG.SEG)]
            ol = [x for x in ol if not DROP.match(x[2])]
            gl = [(pc, t, nm(t)) for pc, t in zcode.insns(gcode, lo, hi, CFG.SEG)]
            import difflib
            sm = difflib.SequenceMatcher(None, [x[2] for x in ol], [x[2] for x in gl], autojunk=False)
            omap = {}
            for tag, i1, i2, j1, j2 in sm.get_opcodes():
                if tag == 'equal' and i2 - i1 >= int(os.environ.get('DN_MIN', '4')):
                    for d in range(i2 - i1):
                        omap[i1 + d] = j1 + d
            pcs = [x[0] for x in ol]
            # all relocations in the function
            for w in range(s, e, 2):
                r = int.from_bytes(rel[w:w + 2], 'big')
                if not (r & 8):
                    continue
                sym = o.symidx.get(r >> 4)
                if not sym or (r & 7) != 0:
                    continue
                ix = bisect.bisect_right(pcs, w) - 1
                if ix < 0:
                    continue
                add = int.from_bytes(oc[w:w + 2], 'big')
                if add >= 0x8000:
                    add -= 0x10000
                if ix not in omap:
                    continue
                t = gl[omap[ix]][1]
                hit = None
                for rx in ADDR:
                    m = rx.search(t)
                    if m:
                        hit = (int(m.group(1)), int(m.group(2), 16) - add)
                        break
                if not hit:
                    none.add(sym['name'])
                    continue
                nmx = sym['name']
                cnt.setdefault(nmx, {}).setdefault(hit, []).append(k)
    for nmx, d in cnt.items():
        best = max(d, key=lambda h: len(d[h]))
        res[nmx] = best
        if len(d) > 1:
            conf.append((nmx, dict((('%d:%04x' % (h[0], h[1] & 0xffff)), sorted(set(v))[:3]) for h, v in d.items())))
    for nmx in sorted(res):
        print('%s %d %04x' % (nmx, res[nmx][0], res[nmx][1] & 0xffff))
    for c in conf:
        print('CONFLICT', c, file=sys.stderr)
    miss = sorted(x for x in none if x not in res)
    print('no address: %s' % ' '.join(miss), file=sys.stderr)
    if wf:
        open(wf, 'w').write(''.join('%s %d %04x\n' % (n, v[0], v[1] & 0xffff) for n, v in sorted(res.items())))


if __name__ == '__main__':
    main()
