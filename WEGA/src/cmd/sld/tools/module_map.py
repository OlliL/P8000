#!/usr/bin/env python3
"""module_map.py - assign the functions of /bin/sld (own_functions.txt) to the C files of the program.

The data area of sld starts with the SCCS strings of the C files in link order (copyname.c, dump.c, ext.c, ld.c, ldm.c, printname.c,
swap.c, symbols.c, then the data of the libc objects). The code of the files is in the same order. A function belongs to the file whose
data range holds the string literals it uses; references to variables of other files (extern) are no witness. Strings = runs of
printable characters ended by 0 in the data area.
Output: one line per function with the files of its string references and of its other data references.
Usage: module_map.py [-v]
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import unlink_seg as U  # noqa: E402
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
FILES = [('copyname', 0x2c), ('dump', 0x78), ('ext', 0x38a), ('ld', 0x448), ('ldm', 0xf60), ('printname', 0x14fe),
         ('swap', 0x156c), ('symbols', 0x159a), ('libc', 0x17ba)]
DATA_END = 0x1BB2


def file_of(off):
    r = None
    for n, a in FILES:
        if off >= a:
            r = n
    return r


def string_map(data):
    """offsets that are part of a string literal (printable run of at least 2 characters ended by 0)"""
    st = set()
    i = 0
    n = len(data)
    while i < n:
        j = i
        while j < n and (32 <= data[j] < 127 or data[j] in (9, 10)):
            j += 1
        if j - i >= 2 and j < n and data[j] == 0:
            st.update(range(i, j + 1))
            i = j + 1
        else:
            i += 1
    return st


def main():
    s = SOut(open(PROG, 'rb').read())
    code = s.segs[0]['code']
    prog = bytes(s.image[:code])
    data = bytes(s.image[code:])
    strs = string_map(data)
    rows = []
    for l in open(os.path.join(HERE, 'own_functions.txt')):
        if l[0] == '#' or not l.strip():
            continue
        f = l.split()
        a, b = int(f[0], 16), int(f[2], 16)
        if a < 0x56:
            continue
        sref, vref, bss1, bss2, calls = [], [], [], [], []
        for pc, i in U.decode_range(prog, True, a, b):
            t = U.z8k.text(i)
            for m in re.finditer(r'<<1>>%([0-9A-F]{4})', t):
                o = int(m.group(1), 16)
                (sref if o in strs else vref if o < DATA_END else bss1).append(o)
            for m in re.finditer(r'#%0100([0-9A-F]{4})', t):
                o = int(m.group(1), 16)
                (sref if o in strs else vref if o < DATA_END else bss1).append(o)
            for m in re.finditer(r'<<2>>%([0-9A-F]{4})', t):
                bss2.append(int(m.group(1), 16))
        rows.append((a, b, f[1], sorted({file_of(o) for o in sref}), sorted({file_of(o) for o in vref}), bss1, bss2))
    return rows, data, strs


if __name__ == '__main__':
    rows, data, strs = main()
    for a, b, n, sf, vf, bss1, bss2 in rows:
        print('%04x-%04x %-15s str:%-12s var:%-22s bss1:%s bss2:%s' % (
            a, b, n, ','.join(sf) or '-', ','.join(vf) or '-',
            ('%04x-%04x' % (min(bss1), max(bss1))) if bss1 else '-', ('%04x-%04x' % (min(bss2), max(bss2))) if bss2 else '-'))
