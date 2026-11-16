#!/usr/bin/env python3
"""Look into and patch an image of a DCP (MS-DOS FAT12) floppy, for the cases of the corpora of the dos programs
(compare/dos*). The geometry is read from the boot sector as dosdev.c does (floppy only).

  dcpimg.py free IMG               number of free clusters of the FAT (entries 2.. that are 0)
  dcpimg.py chain IMG NAME         the clusters of the file NAME of the root directory (11 characters, upper case, no dot
                                   between name and extension: 'F1      TXT' or 'F1.TXT')
  dcpimg.py attr IMG NAME HEX      set the attribute byte of the root entry NAME
  dcpimg.py label IMG TEXT         a volume label entry (attribute 08) in the first free root entry
  dcpimg.py entries IMG [NAME]     the raw root entries in use (name, attribute, first cluster, size); with a directory
                                   NAME (of the root) the entries of its first cluster
  dcpimg.py fillfat IMG            every FAT entry from 2 on that the FAT sectors can hold = FFF (the disk is full)
  dcpimg.py setfat IMG N VALUE     FAT entry N (all copies) = VALUE
  dcpimg.py fatentry IMG N         FAT entry N
  dcpimg.py mkhd OUT TYPE[:BPS] [NAME=HOSTFILE ...]
                                   an image made here, not by dosformat: TYPE 0 a bare file system as a floppy has it, 1 or 4 a
                                   hard disk (sector 0 holds a partition table with one partition of that type, 1 = FAT12,
                                   4 = FAT16, starting at sector 1; the way dosdev.c reads it); the file system has BPS bytes
                                   per sector (default 512) and per cluster (one sector), the files in the root directory
"""
import struct
import sys


def geometry(d):
    bps, spc, rsv, nfat, ndir, tot = struct.unpack_from('<HBHBHH', d, 11)
    spf = struct.unpack_from('<H', d, 22)[0]
    fat1 = rsv * bps
    root = (rsv + nfat * spf) * bps
    data = root + ndir * 32
    return dict(bps=bps, clust=spc * bps, fat1=fat1, nfat=nfat, spf=spf, root=root, ndir=ndir, data=data, tot=tot)


def fatget(d, g, n):
    o = g['fat1'] + n * 3 // 2
    v = d[o] | (d[o + 1] << 8)
    return (v >> 4) if n & 1 else (v & 0xfff)


def fatset(d, g, n, val):
    for k in range(g['nfat']):
        o = g['fat1'] + k * g['spf'] * g['bps'] + n * 3 // 2
        v = d[o] | (d[o + 1] << 8)
        v = (v & 0x000f) | (val << 4) if n & 1 else (v & 0xf000) | val
        d[o] = v & 255
        d[o + 1] = v >> 8


def nclusters(g):
    return (g['tot'] * g['bps'] - g['data']) // g['clust']


def name11(s):
    if '.' in s:
        a, b = s.split('.', 1)
        return (a.upper().ljust(8) + b.upper().ljust(3))[:11]
    return s.upper().ljust(11)[:11]


def mkhd(out, typ, files, bps=512, ndir=32):
    """TYPE 0: a bare file system as a floppy has it (FAT12, the first byte of the image is the jump EB); 1 or 4: a hard disk,
    sector 0 holds a partition table with one partition of that type (1 FAT12, 4 FAT16) starting at sector 1. The file system
    has 1 sector of BPS bytes per cluster, 2 FATs, NDIR root entries and as many sectors as the files need."""
    rsv, nfat = 1, 2
    bodies = [(name11(n), open(f, 'rb').read()) for n, f in (x.split('=', 1) for x in files)]
    need = sum(-(-len(b) // bps) for _, b in bodies)
    rootsec = ndir * 32 // bps
    wide = typ == 4
    spf = 1
    while True:
        tot = rsv + nfat * spf + rootsec + need + 2
        if spf * bps >= ((tot + 2) * 2 if wide else (tot + 2) * 3 // 2 + 3):
            break
        spf += 1
    root = rsv + nfat * spf
    data = root + rootsec
    fs = bytearray(tot * bps)
    fs[0:3] = b'\xeb\x34\x90'
    fs[3:11] = b'WEGA-DOS'
    struct.pack_into('<HBHBHHBH', fs, 11, bps, 1, rsv, nfat, ndir, tot, 0xf8, spf)
    struct.pack_into('<HH', fs, 24, 9, 2)
    fs[510:512] = b'\x55\xaa' if bps >= 512 else b''
    eoc = 0xffff if wide else 0xfff
    fat = {0: 0xfff8 if wide else 0xff8, 1: eoc}
    nxt = 2
    for i, (nm, body) in enumerate(bodies):
        n = -(-len(body) // bps)
        first = nxt if n else 0
        for k in range(n):
            c = nxt + k
            fat[c] = c + 1 if k < n - 1 else eoc
            chunk = body[k * bps:(k + 1) * bps]
            fs[(data + c - 2) * bps:(data + c - 2) * bps + len(chunk)] = chunk
        nxt += n
        o = root * bps + 32 * i
        fs[o:o + 11] = nm.encode()
        fs[o + 11] = 0x20
        struct.pack_into('<HH', fs, o + 22, (8 << 11) | (7 << 5) | 3, (9 << 9) | (3 << 5) | 5)
        struct.pack_into('<HI', fs, o + 26, first, len(body))
    for k in range(nfat):
        base = (rsv + k * spf) * bps
        for c, v in fat.items():
            if wide:
                struct.pack_into('<H', fs, base + 2 * c, v)
            else:
                o = base + c * 3 // 2
                w = fs[o] | (fs[o + 1] << 8)
                w = (w & 0x000f) | (v << 4) if c & 1 else (w & 0xf000) | v
                fs[o], fs[o + 1] = w & 255, w >> 8
    if typ == 0:
        open(out, 'wb').write(bytes(fs))
        return
    mbr = bytearray(512)
    mbr[0x1be + 4] = typ
    struct.pack_into('<II', mbr, 0x1be + 8, 1, tot)
    mbr[510:512] = b'\x55\xaa'
    open(out, 'wb').write(bytes(mbr) + bytes(fs))


def main(a):
    if a[0] == 'mkhd':
        t, _, b = a[2].partition(':')
        return mkhd(a[1], int(t), a[3:], int(b) if b else 512)
    cmd, img = a[0], a[1]
    d = bytearray(open(img, 'rb').read())
    g = geometry(d)
    root = [g['root'] + 32 * i for i in range(g['ndir'])]
    if cmd == 'free':
        print(sum(1 for n in range(2, nclusters(g) + 2) if fatget(d, g, n) == 0))
    elif cmd == 'fillfat':
        for n in range(2, g['spf'] * g['bps'] * 2 // 3):
            fatset(d, g, n, 0xfff)
        open(img, 'wb').write(d)
    elif cmd == 'fatentry':
        print(hex(fatget(d, g, int(a[2], 0))))
    elif cmd == 'setfat':
        fatset(d, g, int(a[2], 0), int(a[3], 0))
        open(img, 'wb').write(d)
    elif cmd == 'chain':
        want = name11(a[2]).encode()
        for o in root:
            if d[o:o + 11] == want:
                n = struct.unpack_from('<H', d, o + 26)[0]
                out = []
                while 2 <= n < 0xff8 and len(out) < 5000:
                    out.append(n)
                    n = fatget(d, g, n)
                print(' '.join(map(str, out)))
                return
        sys.exit('not found')
    elif cmd == 'attr':
        want = name11(a[2]).encode()
        for o in root:
            if d[o:o + 11] == want:
                d[o + 11] = int(a[3], 16)
                open(img, 'wb').write(d)
                return
        sys.exit('not found')
    elif cmd == 'label':
        for o in root:
            if d[o] in (0, 0xe5):
                d[o:o + 32] = bytes(32)
                d[o:o + 11] = a[2].upper().ljust(11)[:11].encode()
                d[o + 11] = 8
                open(img, 'wb').write(d)
                return
        sys.exit('root full')
    elif cmd == 'entries':
        base = root
        if len(a) > 2:
            want = name11(a[2]).encode()
            first = [struct.unpack_from('<H', d, o + 26)[0] for o in root if d[o:o + 11] == want][0]
            b0 = g['data'] + (first - 2) * g['clust']
            base = [b0 + 32 * i for i in range(g['clust'] // 32)]
        for o in base:
            if d[o] == 0:
                break
            print("'%s' %02x %d %d" % (bytes(d[o:o + 11]).decode('latin1').encode('ascii', 'backslashreplace').decode(), d[o + 11], struct.unpack_from('<H', d, o + 26)[0],
                                     struct.unpack_from('<I', d, o + 28)[0]))
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main(sys.argv[1:])
