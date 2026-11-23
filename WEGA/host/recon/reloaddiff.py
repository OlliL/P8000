#!/usr/bin/env python3
"""reloaddiff.py BINARY OBJ...: per function of OBJ (located in BINARY like funcfind), the reload events (paramreload.py)
of the original against those of the object; prints the functions where they differ (a function located wrongly by its
first instructions can show up too: check the address)"""
import sys, difflib
import os
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
sys.path.insert(0, HERE)
import libcver as L
import paramreload as P
from sout import SOut
s, prog, raw = L.load_program(sys.argv[1])
texts = [t for p, t in prog]
idx = {}
for i in range(len(texts) - 2):
    idx.setdefault(tuple(texts[i:i + 3]), []).append(i)
def events(fn):
    out = []
    P.analyse(fn, out, '')
    return [(e[0], e[4], L.norm(e[6])) for e in out if e[0] == 'R']
for f in sys.argv[2:]:
    o = SOut(open(f, 'rb').read())
    img = bytearray(o.image)
    code = o.segs[0]["code"]
    rawo = L.decode(bytes(img), code, False)
    for a in range(0, len(o.reloc) - 1, 2):
        if o.reloc[a] or o.reloc[a + 1]: img[a:a + 2] = b"\xff\xff"
    ins = [(p, L.norm(t)) for p, t in L.decode(bytes(img), code, False)]
    syms = sorted({(y.get('value', 0), y['name']) for y in o.symbols if (y['type'] & 0x1f) == 2 and y.get('value', 0) < code})
    for k, (v, n) in enumerate(syms):
        e = syms[k + 1][0] if k + 1 < len(syms) else code
        seq = [t for p, t in ins if v <= p < e]
        if len(seq) < 3: continue
        cand = idx.get(tuple(seq[:3]), [])
        if not cand: continue
        best = None
        for i in cand[:20]:
            r = difflib.SequenceMatcher(None, seq, texts[i:i + len(seq)], autojunk=False).ratio()
            if best is None or r > best[0]: best = (r, i)
        i = best[1]
        # original function: from i to its ret
        j = i
        while j < len(raw) and not raw[j].startswith('ret'): j += 1
        fo = [(prog[x][0], raw[x]) for x in range(i, j + 1)]
        fb = [(p, t) for p, t in rawo if v <= p < e]
        eo, eb = events(fo), events(fb)
        if eo != eb:
            exact = texts[i:i + len(seq)] == seq
            print('%s %s %s orig@%04x' % (f.split('/')[-1], n, 'EXACT?' if exact else 'diff', prog[i][0]))
            for x in eo:
                if x not in eb: print('   orig only  r%d %s' % (x[1], x[2]))
            for x in eb:
                if x not in eo: print('   built only r%d %s' % (x[1], x[2]))
