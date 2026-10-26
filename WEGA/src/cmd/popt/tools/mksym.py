#!/usr/bin/env python3
"""mksym.py SRC DST - copies the popt sources to DST with the function heads made global (no 'static' on function heads
and on the prototypes of the functions), so that the symbol table of the linked binary lists every function.
Only the storage class of the functions changes; the statics of variables stay. The code is the same (checked by
the disassembly: only the names of the call targets differ). Used by the Makefile target 'symbuild' and locate.py.
"""
import os
import re
import shutil
import sys

src, dst = sys.argv[1], sys.argv[2]
os.makedirs(dst, exist_ok=True)
for f in os.listdir(src):
    if not f.endswith(('.c', '.h')):
        continue
    if f in ('z21.c', 'z22.c', 'z23.c'):
        L = open(os.path.join(src, f)).read().split('\n')
        out = []
        for i, l in enumerate(L):
            nxt = L[i + 1] if i + 1 < len(L) else ''
            if re.match(r'^static\s+[\w\s\*]*$', l) and re.match(r'^\w+\s*\(', nxt) and not nxt.rstrip().endswith(';'):
                l = re.sub(r'^static\s+', '', l)
            elif re.match(r'^static\s+(\w+\s*\**\s*)?\w+\s*\(', l) and not l.rstrip().endswith(';'):
                l = re.sub(r'^static\s+', '', l)
            elif re.match(r'^static\s+[\w\s\*]*\w+\(\);\s*$', l):
                l = re.sub(r'^static\s+', '', l)
            out.append(l)
        open(os.path.join(dst, f), 'w').write('\n'.join(out))
    else:
        shutil.copy(os.path.join(src, f), os.path.join(dst, f))
print('symbol copy in', dst)
