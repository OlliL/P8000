#!/usr/bin/env python3
"""olist.py NAME...: the instructions of functions of the original /lib/popt, with their addresses.

The ranges come from findings.json; run in src/cmd/popt or anywhere (the configuration is found by the launcher).
"""
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
os.environ['RECON_PROG'] = HERE
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon'))
import progcfg  # noqa: E402
import zcode  # noqa: E402

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
from sout import SOut  # noqa: E402

g = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
gc = bytes(g.image)[:g.segs[0]['code']]
F = json.load(open(CFG.FINDINGS))['functions']
for fn in sys.argv[1:]:
    f = F[fn]
    lo = int(f['orig'], 16)
    print('==', fn, f['file'])
    for a, t in zcode.insns(gc, lo, lo + f['size_orig'], CFG.SEG):
        print('%04x %s' % (a, t))
