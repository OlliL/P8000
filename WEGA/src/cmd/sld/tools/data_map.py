#!/usr/bin/env python3
"""data_map.py - classify the data area of /bin/sld per C file: SCCS string, strings, jump tables, other (initialized variables).

Usage: data_map.py [FILE.c]
Strings: printable runs ended by 0 (start at any address, the next item is aligned to 2). Jump tables: runs of long words 0000:XXXX
with XXXX an even code address of the file. The rest is shown as words; names of our variables at these addresses (var_addr.txt)
are added.
"""
import re
import sys
import os

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import ldsrc  # noqa: E402
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
RANGES = {'copyname.c': (0x2c, 0x78), 'dump.c': (0x78, 0x38a), 'ext.c': (0x38a, 0x448), 'ld.c': (0x448, 0xf60), 'ldm.c': (0xf60, 0x14fe),
          'printname.c': (0x14fe, 0x156c), 'swap.c': (0x156c, 0x159a), 'symbols.c': (0x159a, 0x17ba)}


def names_at():
    out = {}
    for l in open(os.path.join(HERE, 'var_addr.txt')):
        p = l.split()
        if p[1] == '-':
            continue
        best = None
        for t in p[1:]:
            m = re.match(r'(\d):([0-9a-f]+)\((\d+)\)', t)
            if m and (best is None or int(m.group(3)) > best[2]):
                best = (int(m.group(1)), int(m.group(2), 16), int(m.group(3)))
        if best and best[0] == 1:
            out.setdefault(best[1], []).append(p[0])
    return out


def classify(data, a, b, code_lo, code_hi):
    items = []
    i = a
    while i < b:
        # jump table: long words 0000:XXXX
        j = i
        while j + 4 <= b and data[j:j + 2] == b'\0\0' and code_lo <= int.from_bytes(data[j + 2:j + 4], 'big') < code_hi:
            j += 4
        if j > i:
            items.append((i, j, 'table'))
            i = j
            continue
        j = i
        while j < b and (32 <= data[j] < 127 or data[j] in (9, 10)):
            j += 1
        if j - i >= 2 and j < b and data[j] == 0 and not (j - i == 2 and data[i:i + 2] == b'\0\0'):
            items.append((i, j + 1, 'str'))
            i = (j + 2) & ~1
            continue
        items.append((i, i + 2, 'word'))
        i += 2
    return items


def main():
    s = SOut(open(PROG, 'rb').read())
    data = bytes(s.image[s.segs[0]['code']:])
    nm = names_at()
    want = sys.argv[1:] or list(RANGES)
    for f in want:
        a, b = RANGES[f]
        lo, hi = [(x, y) for g, x, y in ldsrc.modules() if g == f][0]
        print('==', f, hex(a), hex(b))
        for x, y, k in classify(data, a, b, lo, hi):
            if k == 'str':
                print('%04x str   %r' % (x, data[x:y - 1].decode('latin1')))
            elif k == 'table':
                print('%04x table %d entries' % (x, (y - x) // 4))
            else:
                print('%04x word  %04x   %s' % (x, int.from_bytes(data[x:x + 2], 'big'), ' '.join(nm.get(x, []))))


if __name__ == '__main__':
    main()
