#!/usr/bin/env python3
"""own_table_seg2.py - the first findings.json of a segmented program with several code segments (gopt: segments 4 and 5).

progcfg CODESEGS = {segno: (image offset, size)}. Function starts: call targets (proven) and the starts after an unconditional
ret or jump that no jump reaches (candidate; includes switch-table targets, so a guess). Names f<SEG>_<OFFSET> until named.
An address is written SEG:OFFSET. Existing entries (by orig) are kept. Usage: RECON_PROG=NAME own_table_seg2.py
"""
import json
import os
import sys

import progcfg

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
import z8k  # noqa: E402
from sout import SOut  # noqa: E402


def cond(i):
    return any(o.kind == 'cc' for o in i.ops)


def main():
    s = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    ins = {}
    for sg, (off, n) in CFG.CODESEGS.items():
        b = z8k.Buf(bytes(s.image[off:off + n]), 0)
        pc = 0
        while pc < n:
            i = z8k.decode(b, pc, sg)
            if i is None:
                pc += 2
                continue
            ins[(sg, pc)] = i
            pc = i.end
    proven, bt = set(), set()
    for (sg, pc), i in ins.items():
        for o in i.ops:
            t = None
            if o.kind == 'rel':
                t = (sg, o.a)
            elif o.kind == 'da' and o.a[0] in CFG.CODESEGS:
                t = (o.a[0], o.a[1])
            if t is None:
                continue
            if i.mn in ('call', 'calr'):
                proven.add(t)
            elif i.mn in ('jr', 'jp', 'jpr', 'djnz', 'dbjnz'):
                bt.add(t)
    cand = set()
    for sg in CFG.CODESEGS:
        prev = True
        for k in sorted(k for k in ins if k[0] == sg):
            if prev and k not in bt:
                cand.add(k)
            i = ins[k]
            prev = i.mn in ('ret', 'jp', 'jr') and not cond(i)
    starts = sorted(proven | cand)
    fj = json.load(open(CFG.FINDINGS)) if os.path.exists(CFG.FINDINGS) else {
        'schema': 'functions: name -> orig (SEG:OFFSET), size_orig (to the next start), status (open, exact), kind (proven: a call '
                  'target; candidate: after an unconditional ret/jump, no jump reaches it), module (a guess unless noted), '
                  'note (by hand: an odd source form, a guessed name, dead code).',
        'compiler': '?', 'binary': '/' + CFG.ORIG_BIN, 'functions': {}}
    have = {f['orig'] for f in fj['functions'].values()}
    for n, (sg, pc) in enumerate(starts):
        nxt = [p for q, p in starts if q == sg and p > pc]
        end = nxt[0] if nxt else CFG.CODESEGS[sg][1]
        key = '%d:%04x' % (sg, pc)
        if key in have:
            continue
        fj['functions']['f%d_%04x' % (sg, pc)] = {'orig': key, 'size_orig': end - pc, 'status': 'open',
                                                   'kind': 'proven' if (sg, pc) in proven else 'candidate'}
    json.dump(fj, open(CFG.FINDINGS, 'w'), indent=1)
    print('%d functions (%d proven)' % (len(fj['functions']), len(proven)))


if __name__ == '__main__':
    main()
