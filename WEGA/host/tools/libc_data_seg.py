#!/usr/bin/env python3
"""libc_data_seg.py - add the data words of the libc part of /bin/sld to the segmented module sources.

Each module source (src/lib/oldslibc/<object>/<version>.s, version e.g. 0_1) refers to its data as <<1>>%XXXX (segment 1 address).
This tool groups the referenced words into spans (gap up to GAP bytes), names each span _dXXXX (XXXX = start
offset), rewrites the references to _dXXXX+off and appends a .data section with the words of the original.
The words are read from /bin/sld (data offset = image offset of the data area).
Usage: libc_data_seg.py SEGDIR
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from sout import SOut  # noqa: E402

PROG = '/mnt/share/cvs/P8000/tmp/wega31_root/bin/sld'
GAP = 0x10
REF = re.compile(r'<<1>>%([0-9A-F]{4})')


def bss_ref(o, n):
    return '_bss1+%d' % (o - n)


def main():
    segdir = sys.argv[1]
    s = SOut(open(PROG, 'rb').read())
    data = bytes(s.image)[s.segs[0]['code']:]
    for mod in sorted(os.listdir(segdir)):
        srcs = [f for f in sorted(os.listdir(os.path.join(segdir, mod))) if f.endswith('.s')] if os.path.isdir(os.path.join(segdir, mod)) else []
        if not srcs:
            continue
        src = os.path.join(segdir, mod, srcs[0])
        text = open(src).read()
        refs = sorted({int(m, 16) for m in REF.findall(text)})
        if not refs:
            continue
        # offsets behind the data area are the bss of segment 1: a linker symbol, zero at start
        bss = [r for r in refs if r >= len(data)]
        refs = [r for r in refs if r < len(data)]
        spans = []
        for r in refs:
            if spans and r - spans[-1][1] <= GAP:
                spans[-1][1] = r + 2
            else:
                spans.append([r, r + 2])
        names = {}
        out = []
        for lo, hi in spans:
            name = '_d%04X' % lo
            for o in range(lo, hi, 2):
                names[o] = (name, o - lo)
        lines = text.split('\n')
        new = []
        for ln in lines:
            # references behind the data area (bss of segment 1) stay absolute, as in the original
            ln = REF.sub(lambda m: m.group(0) if int(m.group(1), 16) >= len(data)
                         else '%s+%d' % names[int(m.group(1), 16)], ln)
            new.append(ln)
        body = '\n'.join(new).rstrip('\n') + '\n'
        for lo, hi in spans if spans else []:
            if lo == spans[0][0]:
                body += '\n\t.data\n'
            body += '\n_d%04X::\n' % lo
            for o in range(lo, hi, 2):
                body += '\t.word\t%%%04X\n' % (data[o] << 8 | data[o + 1])
        open(src, 'w').write(body)
        print('%-8s spans %s' % (mod, ' '.join('%04X-%04X' % (a, b) for a, b in spans)))


if __name__ == '__main__':
    main()
