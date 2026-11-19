#!/usr/bin/env python3
"""srcorder.py - put the functions of a module into the order of the original (address), so the link places them as the original.

Usage: RECON_PROG=NAME srcorder.py [-n] FILE.c ...        (-n: only print the order)

A function chunk: from its leading comment / type lines (no blank line between) to the closing `}` at column 0. Only the
functions that have an entry with this `file` in findings.json move (the case blocks and the unnamed pieces do not); the text
between them (declarations, data) stays where it is: the chunks are only permuted among the places of the chunks.
"""
import json
import os
import re
import sys

import progcfg

CFG = progcfg.load()
FN = json.load(open(CFG.FINDINGS))['functions']


def key(k):
    sg, a = FN[k]['orig'].split(':')
    return int(sg), int(a, 16)


def main():
    a = sys.argv[1:]
    dry = '-n' in a
    for path in [x for x in a if x != '-n']:
        base = os.path.basename(path)
        lines = open(path).read().split('\n')
        names = {k for k, v in FN.items() if v.get('file') == base and not re.match(r'(case block|tail) of', v.get('note', ''))
                 and not re.match(r'f\d_', k)}
        chunks = []
        for i, l in enumerate(lines):
            m = re.match(r'(\w+)\(', l)
            if m and m.group(1) in names:
                s = i
                while s > 0 and lines[s - 1].strip() and not re.match(r'[}#]', lines[s - 1]) and not lines[s - 1].rstrip().endswith(';'):
                    s -= 1
                e = i
                while not lines[e].startswith('}'):
                    e += 1
                chunks.append((s, e, m.group(1)))
        order = sorted(chunks, key=lambda c: key(c[2]))
        cur = [c[2] for c in chunks]
        new = [c[2] for c in order]
        print('%s: %d functions, %s' % (base, len(chunks), 'in order' if cur == new else 'reordered'))
        if cur == new or dry:
            continue
        out, pos = [], 0
        for (s, e, n), (s2, e2, n2) in zip(chunks, order):
            out += lines[pos:s] + lines[s2:e2 + 1]
            pos = e + 1
        out += lines[pos:]
        open(path, 'w').write('\n'.join(out))


if __name__ == '__main__':
    main()
