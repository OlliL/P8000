#!/usr/bin/env python3
"""udosfs.py - the UDOS file system (NDOS) on a P8000 floppy image (increment 117).

  udosfs.py ls IMAGE                 the directory: name, type, properties, records, record length, dates
  udosfs.py cat IMAGE NAME           the data of a file on stdout (records, the last one cut to its byte count)

The format (UDOS Systemhandbuch, chapter 5.2; measured on the UDOS system disk and the WEGA start disk):
- An image is RAW, 80 cylinders x 2 sides x 16 sectors of 256 bytes (655360 bytes), cylinder by cylinder, side 0 then
  side 1 (the order of `dskconv -otype raw`).
- A disk address is two bytes: the sector (0..31, counted over both sides: 16..31 are side 1) and the track (cylinder).
- The directory is a file whose descriptor lies at sector 0, track 16H; its first entry names it ("DIRECTORY").
- A directory record holds entries: a byte (bits 0-5 the length of the name, bit 7 secret), the name, the address of
  the file's descriptor; FFH after the last entry of the record.
- A descriptor (256 bytes): 06/07 back pointer to the directory record, 08/09 first data record, 0A/0B last data record,
  0C type/subtype, 0D/0E number of records, 0F/10 record length, 11/12 block length, 13 properties, 14/15 entry point,
  16/17 bytes in the last record, 18-1F date of creation, 20-27 date of last change, 28.. segments (type P), 7A-7F low
  and high address, stack size (type P), 80/81 the first pointer sector.
- A pointer sector (256 bytes): two-byte addresses of data records from byte 00 (in the first pointer sector bytes
  00/01 are the address of the descriptor), FA/FB the offset of the last entry, FC/FD the previous pointer sector,
  FE/FF the next one (FFFFH: none).
Numbers of two bytes are low byte first (Z80 order: the record length 100H is stored 00 01); a disk address is
(sector, track)."""
import sys

SECT = 256
NSECT = 32          # sectors per track address, both sides
DIR_DESC = (0x00, 0x16)


def le(b, o):
    return b[o] | (b[o + 1] << 8)


class Image:
    def __init__(self, path):
        self.data = bytearray(open(path, 'rb').read())

    def sector(self, addr):
        s, t = addr
        if s >= NSECT or t >= 80:
            raise ValueError('bad disk address %02x%02x' % (s, t))
        o = (t * NSECT + s) * SECT
        return bytes(self.data[o:o + SECT])

    def records(self, desc):
        """the addresses of the data records of a file, in order, from its pointer sectors"""
        out = []
        p = (desc[0x80], desc[0x81])
        first = True
        while p != (0xFF, 0xFF):
            ps = self.sector(p)
            last = le(ps, 0xFA)
            for o in range(2 if first else 0, last + 2, 2):
                out.append((ps[o], ps[o + 1]))
            first = False
            p = (ps[0xFE], ps[0xFF])
        n = le(desc, 0x0D)
        return out[:n]

    def read(self, desc):
        """the data of a file: its records (a record of 80H uses half a sector; longer records span sectors)"""
        rl = le(desc, 0x0F)
        recs = self.records(desc)
        out = bytearray()
        for a in recs:
            if rl <= SECT:
                out += self.sector(a)[:rl]
            else:	# a record of several sectors: consecutive sectors from its address
                s, t = a
                for k in range(rl // SECT):
                    out += self.sector(((s + k) % NSECT, t + (s + k) // NSECT))
        cnt = le(desc, 0x16)
        if recs and cnt:
            out = out[:len(out) - rl + cnt]
        return bytes(out)

    def directory(self):
        """[(name, secret, descriptor address)]"""
        d = self.sector(DIR_DESC)
        entries = []
        for a in self.records(d):
            r = self.sector(a)
            i = 0
            while i < SECT and r[i] != 0xFF:
                n = r[i] & 0x3F
                if n == 0:
                    break
                name = r[i + 1:i + 1 + n].decode('latin-1')
                entries.append((name, bool(r[i] & 0x80), (r[i + 1 + n], r[i + 2 + n])))
                i += 3 + n
        return entries


TYPES = 'BADP'


def ftype(b):
    t = ''.join(c for k, c in enumerate(TYPES) if b & (0x10 << k))
    return '%s%d' % (t or '-', b & 0x0F)


def props(b):
    return ''.join(c for k, c in zip(range(2, 8), 'FRSLEW') if b & (1 << k)) or '-'


def date(b):
    return b[:6].split(b'\xff')[0].split(b'\x00')[0].decode('latin-1')


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    img = Image(sys.argv[2])
    if sys.argv[1] == 'ls':
        for name, secret, da in img.directory():
            d = img.sector(da)
            print('%-32s %-4s %-6s %5d %04x %5d %-8s %-8s' % (name, ftype(d[0x0C]), props(d[0x13]), le(d, 0x0D),
                                                             le(d, 0x0F), le(d, 0x16), date(d[0x18:0x20]),
                                                             date(d[0x20:0x28])))
        return 0
    if sys.argv[1] == 'cat':
        for name, secret, da in img.directory():
            if name == sys.argv[3]:
                sys.stdout.buffer.write(img.read(img.sector(da)))
                return 0
        print('%s: not found' % sys.argv[3], file=sys.stderr)
        return 1
    print(__doc__)
    return 2


if __name__ == '__main__':
    sys.exit(main())
