#!/usr/bin/env python3
"""rawblock.py OUT.s - the own part of /lib/cparse as one raw module (step 3 of host/HOWTO_reconstruct.md).

Text 0x002a..0x9f06 and data 0x0002..0x41a4 of the original as .byte lines, the label _main at 0x60d8 (crt0 calls it)
and the own bss (0x489a..0xb4f4) as one common. Linked as crt0-codgen.o, OUT.o, cplibc.o it gives /lib/cparse byte for
byte (make block). The output contains the bytes of the original: it is made at build time and never kept in git.
"""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', 'host', 'tools'))
from sout import SOut  # noqa: E402

ROOT = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
TEXT = (0x002a, 0x9f06)
DATA = (0x0002, 0x41a4)
BSS = 0xb4f4 - 0x489a
LABELS = {0x60d8: '_main'}


def rows(b, base, labels):
    out, line = [], []
    for i, x in enumerate(b):
        a = base + i
        if a in labels:
            if line:
                out.append('\t.byte\t' + ','.join(line))
                line = []
            out.append(labels[a] + '::')
        line.append('%%%02x' % x)
        if len(line) == 16:
            out.append('\t.byte\t' + ','.join(line))
            line = []
    if line:
        out.append('\t.byte\t' + ','.join(line))
    return out


def main():
    g = SOut(open(os.path.join(ROOT, 'lib', 'cparse'), 'rb').read())
    img = bytes(g.image)
    c = g.segs[0]['code']
    out = ['\t.comm\t%d, _ownbss' % BSS, '\t.psec', '\t.code']
    out += rows(img[TEXT[0]:TEXT[1]], TEXT[0], LABELS)
    out += ['\t.psec', '\t.data']
    out += rows(img[c + DATA[0]:c + DATA[1]], DATA[0], {})
    open(sys.argv[1], 'w').write('\n'.join(out) + '\n')


if __name__ == '__main__':
    main()
