#!/usr/bin/env python3
"""udosfs.py - the UDOS file system (NDOS) on a P8000 floppy image (increment 117).

  udosfs.py ls IMAGE                 the directory: name, type, properties, records, record length, dates
  udosfs.py cat IMAGE NAME           the data of a file on stdout (records, the last one cut to its byte count)
  udosfs.py format IMAGE [YYMMDD]    an empty UDOS data disk (format_disk)
  udosfs.py put IMAGE NAME TYPE RL FILE [YYMMDD]   a file onto the disk: TYPE A, B or P (A: an end mark FFH is added),
                                     RL the record length (hex), the last record filled with FFH (as the coupling
                                     software writes); an existing file of that name is replaced

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


# ---- writing (the model of the 8-bit part, increment 117) -------------------------------------------------------------
# What NDOS does on a disk is known from the handbook and the sample disks; how it chooses free sectors is not (no table of
# free sectors on the disk: NDOS keeps the allocation in memory). The writer's own rule, documented as such: free sectors
# by the distance of their track from the directory's track 16H (16H, 15H, 17H, 14H, ...; on the samples files lie around
# it), within a track in ascending order; a record longer than 256 bytes takes consecutive free sectors; sector 0 of track
# 0 is not used when it holds something (the boot sector of a system disk).

DIR_TRACK = 0x16


def put_le(b, o, v):
    b[o] = v & 0xFF
    b[o + 1] = (v >> 8) & 0xFF


class Disk(Image):
    """an image that can be written; save() writes it back"""

    def __init__(self, path):
        Image.__init__(self, path)
        self.path = path

    def save(self):
        with open(self.path, 'r+b') as f:
            f.write(self.data)

    def put(self, addr, data):
        s, t = addr
        o = (t * NSECT + s) * SECT
        self.data[o:o + SECT] = bytes(data[:SECT]).ljust(SECT, b'\0')

    def file_sectors(self, da):
        """every sector of a file: descriptor, pointer sectors, data"""
        d = self.sector(da)
        out = [da]
        p = (d[0x80], d[0x81])
        while p != (0xFF, 0xFF):
            out.append(p)
            ps = self.sector(p)
            p = (ps[0xFE], ps[0xFF])
        rl = le(d, 0x0F)
        for a in self.records(d):
            s, t = a
            for k in range(max(1, rl // SECT)):
                out.append(((s + k) % NSECT, t + (s + k) // NSECT))
        return out

    def used(self):
        u = set()
        for name, secret, da in self.directory():
            u.update(self.file_sectors(da))
        if self.sector((0, 0)) != b'\xe5' * SECT:
            u.add((0, 0))
        return u

    def alloc(self, n, run, used):
        """n groups of `run` consecutive sectors not in used (the writer's rule above); [] when there are not enough"""
        order = sorted(range(80), key=lambda t: (abs(t - DIR_TRACK), t > DIR_TRACK))
        out = []
        used = set(used)
        for t in order:
            for s in range(NSECT):
                if len(out) == n:
                    return out
                i = t * NSECT + s
                grp = [i + k for k in range(run)]
                if grp[-1] >= 80 * NSECT or any((g % NSECT, g // NSECT) in used for g in grp):
                    continue
                out.append((s, t))
                used.update((g % NSECT, g // NSECT) for g in grp)
        return out if len(out) == n else []

    def find(self, name):
        for n, secret, da in self.directory():
            if n == name:
                return da
        return None

    def add_entry(self, name, da):
        """the name into the directory (at the end of the last record that has room, else a new record): the address of
        the directory record"""
        d = self.sector(DIR_DESC)
        nb = name.encode('latin-1')
        ent = bytes([len(nb)]) + nb + bytes(da)
        for a in self.records(d):
            r = bytearray(self.sector(a))
            i = 0
            while i < SECT and r[i] != 0xFF and r[i] & 0x3F:
                i += 3 + (r[i] & 0x3F)
            if i + len(ent) + 1 <= SECT:
                r[i:i + len(ent)] = ent
                r[i + len(ent)] = 0xFF
                self.put(a, r)
                return a
        raise OSError(0xD3, 'disk full (directory)')

    def write_file(self, name, ftype, rl, data, count, date, filler=b'\x00\xff', props=0, desc_tail=None):
        """a new file (or the old one cleared, as OPEN for OUTPUT does): data are whole records (the caller pads the last
        one), count the bytes of the last record as the descriptor holds them; desc_tail: descriptor bytes 28H..7FH"""
        old = self.find(name)
        nrec = len(data) // rl
        run = max(1, rl // SECT)
        used = self.used()
        if old is not None:		# OPEN for OUTPUT: its records are deleted; descriptor, first pointer sector, entry stay
            od = self.sector(old)
            keep = [old, (od[0x80], od[0x81])]
            used -= set(self.file_sectors(old)) - set(keep)
            da, ptrs = keep[0], [keep[1]]
        else:
            hdr = self.alloc(2, 1, used)
            if not hdr:
                raise OSError(0xD3, 'disk full')
            da, ptrs = hdr[0], [hdr[1]]
        used |= {da, ptrs[0]}
        need_ptr = max(1, (nrec + 1 + 124) // 125)
        if need_ptr > 1:
            more = self.alloc(need_ptr - 1, 1, used)
            if not more:
                raise OSError(0xD3, 'disk full')
            ptrs += more
            used |= set(more)
        recs = self.alloc(nrec, run, used)
        if nrec and not recs:
            raise OSError(0xD3, 'disk full')
        for i, a in enumerate(recs):
            s, t = a
            for k in range(run):
                self.put(((s + k) % NSECT, t + (s + k) // NSECT), data[i * rl + k * SECT:i * rl + (k + 1) * SECT] if rl >= SECT
                         else data[i * rl:(i + 1) * rl].ljust(SECT, b'\xff'))
        entries = [da] + recs
        for j, p in enumerate(ptrs):
            chunk = entries[j * 125:(j + 1) * 125]
            ps = bytearray(SECT)
            for k, a in enumerate(chunk):
                ps[2 * k], ps[2 * k + 1] = a
            put_le(ps, 0xFA, 2 * (len(chunk) - 1))
            ps[0xFC], ps[0xFD] = da if j == 0 else ptrs[j - 1]
            ps[0xFE], ps[0xFF] = ptrs[j + 1] if j + 1 < len(ptrs) else (0xFF, 0xFF)
            self.put(p, ps)
        old_desc = self.sector(old) if old is not None else None
        dirrec = self.add_entry(name, da) if old is None else tuple(old_desc[6:8])
        d = bytearray(SECT)
        d[6], d[7] = dirrec
        if recs:
            d[8], d[9] = recs[0]
            d[0x0A], d[0x0B] = recs[-1]
        d[0x0C] = ftype
        put_le(d, 0x0D, nrec)
        put_le(d, 0x0F, rl)
        d[0x13] = props
        put_le(d, 0x16, count)
        dt = date.encode('latin-1')[:6] + filler
        d[0x18:0x20] = dt
        d[0x20:0x28] = dt
        if old_desc is not None:		# the date of creation stays
            d[0x18:0x20] = old_desc[0x18:0x20]
        if desc_tail is not None:
            d[0x28:0x80] = desc_tail
        d[0x80], d[0x81] = ptrs[0]
        self.put(da, d)
        return da


def format_disk(path, date):
    """an empty UDOS data disk (no system: sector 0 of track 0 is not written): every sector E5H as after formatting, the
    directory as on every sample disk: descriptor 0/16H, pointer sector 2/16H, 5 records on the sectors 5, 10, 1, 6, 11 of
    track 16H, the first with the entry DIRECTORY, the others empty (FFH, then zeros), type D, properties SLEW, record
    length and block length 100H, dates DATE with the filler FF 00. Modelled on the samples; a disk freshly formatted by
    UDOS has not been compared yet (increment 117)."""
    data = bytearray(b'\xe5' * (80 * NSECT * SECT))
    recs = [(5, 0x16), (10, 0x16), (1, 0x16), (6, 0x16), (11, 0x16)]

    def put(addr, b):
        o = (addr[1] * NSECT + addr[0]) * SECT
        data[o:o + SECT] = bytes(b).ljust(SECT, b'\0')
    d = bytearray(SECT)
    d[6], d[7] = recs[0]
    d[8], d[9] = recs[0]
    d[0x0A], d[0x0B] = recs[-1]
    d[0x0C] = 0x40
    put_le(d, 0x0D, len(recs))
    put_le(d, 0x0F, 0x100)
    put_le(d, 0x11, 0x100)
    d[0x13] = 0xF0
    dt = date.encode('latin-1')[:6] + b'\xff\x00'
    d[0x18:0x20] = dt
    d[0x20:0x28] = dt
    d[0x80], d[0x81] = 2, 0x16
    put(DIR_DESC, d)
    ps = bytearray(SECT)
    for k, a in enumerate([DIR_DESC] + recs):
        ps[2 * k], ps[2 * k + 1] = a
    put_le(ps, 0xFA, 2 * len(recs))
    ps[2 * len(recs) + 2:2 * len(recs) + 4] = b'\xff\xff'	# behind the last entry (every sample directory; not in files)
    ps[0xFC], ps[0xFD] = DIR_DESC
    ps[0xFE], ps[0xFF] = 0xFF, 0xFF
    put((2, 0x16), ps)
    put(recs[0], b'\x89DIRECTORY' + bytes(DIR_DESC) + b'\xff')
    for a in recs[1:]:
        put(a, b'\xff')
    with open(path, 'wb') as f:
        f.write(data)


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
    if sys.argv[1] == 'format':
        format_disk(sys.argv[2], sys.argv[3] if len(sys.argv) > 3 else '890509')
        return 0
    if sys.argv[1] == 'put':
        disk = Disk(sys.argv[2])
        t = {'A': 0x20, 'B': 0x10, 'P': 0x80}[sys.argv[4]]
        rl = int(sys.argv[5], 16)
        body = open(sys.argv[6], 'rb').read()
        if t == 0x20:
            body += b'\xff'
        n = len(body)
        last = n % rl or (rl if n else 0)
        disk.write_file(sys.argv[3], t, rl, body + b'\xff' * (-n % rl), max(0, last - 1) if t == 0x20 else last,
                        sys.argv[7] if len(sys.argv) > 7 else '890509')
        disk.save()
        return 0
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
