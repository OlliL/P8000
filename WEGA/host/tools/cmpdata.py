#!/usr/bin/env python3
"""cmpdata.py: compare the initialised data of a module (compiled by scc) with its range in a binary.

    cmpdata.py OBJECT.o BINARY START END [SEGMENT]

START END is the range of the module in the data segment (default segment 5, hex);
the words that the object has relocated (pointers, addresses) are not compared.
Prints the size of both and the first differences.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sout import SOut

def seg_data(path, segno=None):
    s = SOut(open(path, 'rb').read())
    # data of the first segment table entry with data, in image order
    off = sum(g['code'] for g in s.segs)
    res = []
    for g in s.segs:
        if g['data']:
            res.append((g['segno'], off, g['data']))
        off += g['data']
    return s, res

def main(argv):
    obj, binary = argv[0], argv[1]
    lo, hi = int(argv[2], 16), int(argv[3], 16)
    segno = int(argv[4]) if len(argv) > 4 else 5
    so, od = seg_data(obj)
    sb, bd = seg_data(binary)
    o = None
    for no, off, size in od:
        o = (off, size)
        break
    if o is None:
        print('no data in the object'); return 1
    odata = so.image[o[0]:o[0] + o[1]]
    orel = so.reloc[o[0]:o[0] + o[1]] if so.reloc else b'\0' * o[1]
    # the binary: the data of the segment (file layout: text of all, then data of all)
    boff = sum(g['code'] for g in sb.segs)
    bdata = None
    for g in sb.segs:
        if g['segno'] == segno and g['data']:
            bdata = sb.image[boff:boff + g['data']]
            break
        boff += g['data']
    if bdata is None:
        print('segment %d not found' % segno); return 1
    bd_ = bdata[lo:hi]
    print('object data %d bytes, binary range %d bytes (%04x-%04x)' % (len(odata), len(bd_), lo, hi))
    n = min(len(odata), len(bd_))
    bad = []
    for a in range(0, n):
        w = a & ~1
        if orel[w:w + 2] != b'\0\0':
            continue
        if odata[a] != bd_[a]:
            bad.append(a)
    print('%d differing bytes in the first %d bytes (relocated words skipped)' % (len(bad), n))
    for a in bad[:12]:
        print('  %04x: new %02x orig %02x' % (a, odata[a], bd_[a]))
    # the order of strings, switch tables and variables depends on the compiler: compare the contents too
    import collections
    def hist(data, rel):
        c = collections.Counter()
        for a in range(len(data)):
            if rel[a & ~1:(a & ~1) + 2] == b'\0\0':
                c[data[a]] += 1
        return c
    ho, hb = hist(odata, orel), hist(bd_, b'\0' * len(bd_))
    # the binary side has relocated words too (pointers) that we cannot see: count only what both can show
    diff = sum(((ho - hb) + (hb - ho)).values())
    if bad or len(odata) != len(bd_):
        print('byte histogram differs by %d (pointers and tables in another order show up here)' % diff)
    return 0 if not bad and len(odata) == len(bd_) else 1

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
