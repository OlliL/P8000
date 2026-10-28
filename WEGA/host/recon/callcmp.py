#!/usr/bin/env python3
"""callcmp.py OLD_TRACE NEW_TRACE NEW_NM - find the first function whose calls differ between the original and the rebuilt program.

Traces: `z8k-emu -t PROGRAM ARGS > file` (one line per instruction: `00:PC WORD r0-7 ... sp ...`). NEW_NM: `nm` of the new program
(lines `ADDR T _name`). The original is mapped by own_functions.txt and libc_names.txt. A call is the opcode `call` (5fxx) or `calr` (Dxxx);
the next trace line is the callee. Prints the first divergence of the (name, arguments) sequence with context, then compare the flow
inside that function. Used for the corpus test (sld) (KNOWLEDGE.md, "corpus test")."""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import progcfg  # noqa: E402
CFG = progcfg.load()


def names_new(nm):
    d = {}
    for l in open(nm):
        p = l.split()
        if len(p) == 3 and p[1] in 'Tt':
            d[int(p[0], 16)] = p[2].lstrip('_')
    return d


def names_old():
    d = {}
    for l in open(os.path.join(CFG.DIR, 'own_functions.txt')):
        p = l.split()
        if len(p) > 2 and not l.startswith('#'):
            d[int(p[0], 16)] = p[1]
    for l in open(os.path.join(CFG.DIR, 'libc_names.txt')):
        p = l.split()
        if len(p) >= 2 and re.match(r'^[0-9a-fA-F]+$', p[0]):
            d[int(p[0], 16)] = p[1].lstrip('_')
    return d


def calls(fn, names):
    out, prev = [], None
    for l in open(fn):
        m = re.match(r'00:([0-9a-f]{4}) ([0-9a-f]{4})\s+r0-7 (\w+) (\w+) (\w+) (\w+) (\w+) (\w+) (\w+) (\w+)', l)
        if not m:
            continue
        pc, w = int(m.group(1), 16), int(m.group(2), 16)
        if prev is not None:
            nm = names.get(pc)
            if nm:
                out.append((nm,) + prev)
            prev = None
        if (w >> 8) == 0x5f or (w >> 12) == 0xd:
            prev = tuple(m.group(i) for i in (9, 10, 7, 8))      # r6 r7 r4 r5 at the call
    return out


def main():
    old, new, nm = sys.argv[1:4]
    own = {l.split()[1] for l in open(os.path.join(CFG.DIR, 'own_functions.txt')) if len(l.split()) > 2 and not l.startswith('#')}
    a = [c for c in calls(old, names_old()) if c[0] in own and c[0] != 'main']
    b = [c for c in calls(new, names_new(nm)) if c[0] in own and c[0] != 'main']
    print(len(a), len(b))
    for i, (x, y) in enumerate(zip(a, b)):
        if x[0] != y[0]:
            print('first divergence at call %d' % i)
            for j in range(max(0, i - 8), i + 4):
                print(j, a[j], b[j])
            return
    print('no divergence in the common prefix')


if __name__ == '__main__':
    main()
