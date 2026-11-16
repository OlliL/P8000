#!/usr/bin/env python3
"""imd.py - RAW floppy images of the P8000 to and from ImageDisk (IMD), the format through which they go into MAME and
come back (floptool of MAME reads and writes IMD; CopyQM it only reads).

  imd.py toimd RAW IMD [SECTORS SIZE]    a RAW image (cylinder by cylinder, side 0 then side 1) to IMD
  imd.py toraw IMD RAW                   an IMD image to RAW (sectors in order of their numbers)

Default geometry: the UDOS disks, 80 cylinders, 2 sides, 16 sectors of 256 bytes numbered 1..16, MFM 250 kbit/s (mode
5), as floptool writes the UDOS system disk; SECTORS 9 and SIZE 512 for the WEGA disks (96ds9)."""
import sys

SIZE_CODE = {128: 0, 256: 1, 512: 2, 1024: 3}


def toimd(raw, out, nsec=16, size=256, cyls=80, heads=2, mode=5):
    data = open(raw, 'rb').read()
    b = bytearray(b'IMD 1.18: 01/01/2000 00:00:00\r\nP8000 image (host/tools/imd.py)\r\n\x1a')
    p = 0
    for c in range(cyls):
        for h in range(heads):
            b += bytes([mode, c, h, nsec, SIZE_CODE[size]])
            b += bytes(range(1, nsec + 1))
            for s in range(nsec):
                sec = data[p:p + size].ljust(size, b'\0')
                p += size
                if sec == bytes([sec[0]]) * size:
                    b += bytes([2, sec[0]])	# compressed: every byte the same
                else:
                    b += b'\x01' + sec
    with open(out, 'wb') as f:
        f.write(b)


def toraw(imd, out):
    d = open(imd, 'rb').read()
    p = d.index(b'\x1a') + 1
    tracks = {}
    while p < len(d):
        mode, cyl, head, ns, sz = d[p:p + 5]
        p += 5
        smap = list(d[p:p + ns])
        p += ns
        if head & 0x80:
            p += ns
        if head & 0x40:
            p += ns
        size = 128 << sz
        secs = {}
        for k in range(ns):
            ty = d[p]
            p += 1
            if ty in (1, 3, 5, 7):
                secs[smap[k]] = d[p:p + size]
                p += size
            elif ty in (2, 4, 6, 8):
                secs[smap[k]] = bytes([d[p]]) * size
                p += 1
            else:
                secs[smap[k]] = bytes(size)
        tracks[(cyl, head & 0x0F)] = b''.join(secs[n] for n in sorted(secs))
    with open(out, 'wb') as f:
        for key in sorted(tracks):
            f.write(tracks[key])


def main():
    if len(sys.argv) >= 4 and sys.argv[1] == 'toimd':
        nsec = int(sys.argv[4]) if len(sys.argv) > 4 else 16
        size = int(sys.argv[5]) if len(sys.argv) > 5 else 256
        toimd(sys.argv[2], sys.argv[3], nsec, size)
        return 0
    if len(sys.argv) == 4 and sys.argv[1] == 'toraw':
        toraw(sys.argv[2], sys.argv[3])
        return 0
    print(__doc__)
    return 2


if __name__ == '__main__':
    sys.exit(main())
