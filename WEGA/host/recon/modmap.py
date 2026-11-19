#!/usr/bin/env python3
"""modmap.py - the module of every function of a program without symbols (progcfg CODESEGS, SCCS lines, gopt), from its data.

The data segment holds the strings and tables of every module in link order, each module starting with its SCCS line
(`@[$]NAME`). A function that reads a string or table (a 32-bit immediate #%87xxxxxx or an address in segment 7) of module
M lies in M or reads M's table. The modules lie in the code in link order, so the firm anchors (references into one module only,
none to the commons below `COMMONS`) are cut to the longest non-decreasing run; a function between two anchors of the same
module belongs to it; one between anchors of two modules is written 'A|B' (gap functions, a guess). Writes the field `module` of
findings.json. progcfg: MODULES (one per SCCS line without mfile2.h, in order), DATASEG (7), COMMONS (lo, hi of the shared words).
"""
import bisect
import json
import os
import re
import sys

import progcfg

CFG = progcfg.load()
sys.path.insert(0, progcfg.TOOLS)
import z8k  # noqa: E402
from sout import SOut  # noqa: E402


def main():
    s = SOut(open(os.path.join(CFG.ROOT, CFG.ORIG_BIN), 'rb').read())
    img = bytes(s.image)
    codesz = sum(n for _, n in CFG.CODESEGS.values())
    data = img[codesz:]
    names = [n for n in (m.group(1).decode() for m in re.finditer(rb'@\[\$\]([^\t\0]*)', data))]
    # one entry per module: the SCCS lines of mfile2.h and of included files (CFG.INCLUDED) do not start one
    starts = [(m.start(), m.group(1).decode()) for m in re.finditer(rb'@\[\$\]([^\t\0]*)', data)
              if m.group(1).decode() not in CFG.NOT_MODULE]
    seen = set()
    for n, (o, nm) in enumerate(starts):    # a second module with the same SCCS name (gopt: alias2.c twice) gets a 'b'
        if nm in seen:
            starts[n] = (o, nm.replace('.c', 'b.c'))
        seen.add(nm)
    lo = [o for o, _ in starts]
    fj = json.load(open(CFG.FINDINGS))
    fn = fj['functions']
    order = sorted(fn, key=lambda k: (int(fn[k]['orig'].split(':')[0]), int(fn[k]['orig'].split(':')[1], 16)))
    anchors = []
    for k in order:
        sg, off = fn[k]['orig'].split(':')
        sg, off = int(sg), int(off, 16)
        o, n = CFG.CODESEGS[sg]
        b = z8k.Buf(img[o:o + n], 0)
        pc, refs = off, set()
        while pc < off + fn[k]['size_orig']:
            i = z8k.decode(b, pc, 1)
            if i is None:
                pc += 2
                continue
            for op in i.ops:
                if op.kind in ('da', 'x') and op.a[0] == 7:
                    refs.add(op.a[1])
                elif op.kind == 'imm' and op.b == 32 and (op.a >> 24) == 0x87:
                    refs.add(op.a & 0xFFFF)
            pc = i.end
        refs = [r for r in refs if not (CFG.COMMONS[0] <= r <= CFG.COMMONS[1]) and r >= lo[0]]
        mods = sorted(set(bisect.bisect_right(lo, r) - 1 for r in refs))
        anchors.append(mods)
    firm = [(i, m[0]) for i, m in enumerate(anchors) if len(m) == 1]
    # longest non-decreasing subsequence
    best = [1] * len(firm)
    prev = [-1] * len(firm)
    for a in range(len(firm)):
        for b in range(a):
            if firm[b][1] <= firm[a][1] and best[b] + 1 > best[a]:
                best[a], prev[a] = best[b] + 1, b
    a = max(range(len(firm)), key=lambda x: best[x])
    keep = []
    while a != -1:
        keep.append(firm[a])
        a = prev[a]
    keep.reverse()
    lower, upper = {}, {}
    for i in range(len(order)):
        l = [m for j, m in keep if j <= i]
        u = [m for j, m in keep if j >= i]
        lower[i] = l[-1] if l else 0
        upper[i] = u[0] if u else len(starts) - 1
    dropped = [(order[j], m) for j, m in firm if (j, m) not in keep]
    for i, k in enumerate(order):
        l, u = lower[i], upper[i]
        both = [m for m in anchors[i] if l <= m <= u]
        if l != u and len(both) == 1:     # several modules read, one of them between the firm anchors
            l = u = both[0]
        fn[k]['module'] = starts[l][1] if l == u else '%s|%s' % (starts[l][1], starts[u][1])
    json.dump(fj, open(CFG.FINDINGS, 'w'), indent=1)
    print('modules:', [n for _, n in starts])
    print('firm anchors dropped (not in link order):', dropped)
    cnt = {}
    for k in order:
        cnt[fn[k]['module']] = cnt.get(fn[k]['module'], 0) + 1
    print(cnt)


if __name__ == '__main__':
    main()
