#!/usr/bin/env python3
"""libc_spec.py PROGRAM - address ranges of the shipped libc members identified in PROGRAM (libcver.py).

Prints one line per identified member: NAME shipped START END (the end is the address after its last instruction),
sorted by address. The other pieces (old libc, no shipped member) are listed as well, with their nearest member.
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import libcver  # noqa: E402

path = sys.argv[1]
root = os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root')
lib = os.path.join(root, 'lib', 'libc.a')
mem = libcver.load_members(lib)
s, prog, raw = libcver.load_program(path)
found = libcver.find_members(prog, mem)
rows = []
for name, starts in found.items():
    for st in starts:
        n = len(mem[name])
        end = prog[st + n][0] if st + n < len(prog) else prog[-1][0]
        rows.append((prog[st][0], end, name[:-2]))
for a, e, n in sorted(rows):
    print('%-10s shipped %04x %04x' % (n, a, e))
res = libcver.analyse(path, mem)
for o in res['other']:
    print('%-10s other   %04x %04x  n=%d like %s (%.2f)' % ('?', o['first'], o['last'] + 2, o['n'], o['like'], o['ratio']))
