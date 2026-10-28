#!/usr/bin/env python3
"""check_data.py - compare the data section of every compiled C file with its data range in /bin/sld.

Usage: check_data.py [FILE.c ...]
Relocated words of our object (addresses of variables, strings, functions) are not compared. Prints per file the sizes and the first
differences (our offset, original address, bytes).
"""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import ldsrc  # noqa: E402
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
RANGES = {'copyname.c': (0x2c, 0x78), 'dump.c': (0x78, 0x38a), 'ext.c': (0x38a, 0x448), 'ld.c': (0x448, 0xf60), 'ldm.c': (0xf60, 0x14fe),
          'printname.c': (0x14fe, 0x156c), 'swap.c': (0x156c, 0x159a), 'symbols.c': (0x159a, 0x17ba)}


def main():
    s = SOut(open(PROG, 'rb').read())
    data = bytes(s.image[s.segs[0]['code']:])
    objs, errs = ldsrc.compile_all('-O')
    if errs:
        sys.exit(errs)
    want = sys.argv[1:] or list(RANGES)
    for f in want:
        a, b = RANGES[f]
        o = SOut(open(objs[f], 'rb').read())
        csz = o.segs[0]['code']
        dsz = o.segs[1]['data'] if len(o.segs) > 1 else 0
        mine = bytes(o.image[csz:csz + dsz])
        mask = [bool(o.reloc[csz + k]) if csz + k < len(o.reloc) else False for k in range(dsz)]
        ref = data[a:b]
        # jump tables: long words 0000:code address in both (the addresses differ with the code)
        for k in range(0, min(len(mine), len(ref)) - 3, 2):
            if ref[k:k + 2] == b'\0\0' and 0x56 <= int.from_bytes(ref[k + 2:k + 4], 'big') < 0x71ca and mine[k:k + 2] == b'\0\0' \
                    and int.from_bytes(mine[k + 2:k + 4], 'big') < 0x71ca and int.from_bytes(ref[k + 2:k + 4], 'big') % 2 == 0:
                for j in range(4):
                    mask[k + j] = True
        # pointers into segment 1 or 2 (strings, variables): relocated in our object, absolute in the original
        for k in range(0, min(len(mine), len(ref)) - 3, 2):
            if ref[k:k + 2] in (b'\x01\x00', b'\x02\x00') and (mine[k:k + 4] == b'\0\0\0\0' or mine[k:k + 2] == ref[k:k + 2]):
                for j in range(4):
                    mask[k + j] = True
        bad = []
        for k in range(min(len(mine), len(ref))):
            if not mask[k] and mine[k] != ref[k]:
                bad.append(k)
        print('%-12s original %5d ours %5d  differing bytes %d  bss %s' % (f, len(ref), dsz, len(bad) + abs(len(ref) - len(mine)),
                                                                           [x['bss'] for x in o.segs[1:]]))
        if bad:
            k = bad[0]
            print('   first difference at our offset %d (original 0x%04x): ours %s original %s' % (
                k, a + k, mine[max(0, k - 4):k + 12].hex(), ref[max(0, k - 4):k + 12].hex()))


if __name__ == '__main__':
    main()
