#!/usr/bin/env python3
"""ud8.py - the model of the 8-bit part of the P8000 behind /dev/ud (increment 117).

z8k-emu starts it for an open of /dev/ud (Z8K_DEVTAB: model ud proc:...); its stdin and stdout are the byte stream of the
coupling: what the WEGA program writes to /dev/ud arrives here, what is written here the program reads.

  ud8.py [--drive0 IMAGE] [--drive1 IMAGE] ... [--console FILE] [--date YYMMDD]

It is the coupling software of the 8-bit part (UDOS/WEGA/K.*.S beside WEGA/, koppeldoc) transcribed: it waits for a
request letter and serves it (an unknown letter is dropped, as the request dispatch of K.NEU.S does):
  L  LOAD: receive a UDOS file (putud)        K.ACT.S LOAD
  S  SEND: send a UDOS file (getud)           K.ACT.S SEND
  P  PROM: the EPROM programmer (pburn)       K.PROM.S prom, K.P8000.S
  R  READ, W  WRITE: floppy blocks of 512 bytes (the kernel's floppy driver)   K.BLK.S RBLOCK/BLKRD, WBLOCK/BLKWT
  F  FORMAT, V  VERIFY, C  COPYDISK (format, verify, copydisk)                  K.BLK.S BLKFO
The floppies of the 8-bit part (F V C R W) are the same image files as the drives of the case; the format of a disk is
the one of the minor number of the request (fd(4): the upper 4 bits; K.BLK.S TYP: types 8, 9, 10 by table 6, the
types above 10 as 6). FORMAT writes the disk anew in that format, every sector E5H (the pattern of formatted sectors);
a disk whose image does not have the size of the format cannot be read in it: C4 (sector error). No disk: C2 (not
ready), a drive given read-only (--ro N): C3 (write protected); errors of the source of COPYDISK with bit 5 (E2).
The EPROM programmer (--eprom FILE --eprom-type TYPE [--board 1|2] [--short]): the EPROM in the socket is the file
FILE (its size that of TYPE; made erased, all FFH, when it is missing); board 1 is the old programmer card, board 2 the
new one (K.P8000.S: the types each card takes); --short: the card reports a short circuit (error 2). Programming
behaves as an EPROM: a bit can go from 1 to 0 only; a byte that does not take its value is a programming error (3).
Erasing is done by the case (the file set to FFH: UV light).
The UDOS file system (NDOS) is host/tools/udosfs.py on the image files of the drives. What the 8-bit part prints on its
console (the errors: "File NAME Error XX") goes to the file of --console. The system date of UDOS is --date (default
890509, the date of the directory of the WEGA start disk; UDOS asks for it when it starts).

Measured in MAME (the real coupling software and UDOS, increment 117): a new file without a drive (putud never sends
one) is made on the last drive that has a disk (with disks in drives 0 and 1: drive 1, also when drive 0 has more
room); putud onto an existing file replaces it with the type and record length asked for (no type check: OPNFIL sets
the attributes before OPEN), its date of creation stays; getud changes the date of last change of the file it read
(CLOSE writes the descriptor back). Not modelled: NDOS's choice of free sectors (the writer's own rule of udosfs.py).
"""
import argparse
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'tools'))
import udosfs  # noqa: E402

SOH, STX, ETX, EOT, ACK, NAK, CAN, ESC = 0x01, 0x02, 0x03, 0x04, 0x06, 0x15, 0x18, 0x1B
XFERSZ = 64
BFRLEN = 0x1000
RL = 0x200			# the record length of a file made by L (K.ACT.S RL)
MAGIC = b'\xd6\x07'		# 0D607H as K.ACT.S stores it (high byte first) in front of a descriptor
F_BINARY, F_FILDES, F_EXEC = 0x01, 0x04, 0x10	# the flag byte (K.ACT.S BINARY, FILDES, EXEC)
T_P, T_A, T_B = 0x80, 0x20, 0x10		# UDOS file types


class Stop(Exception):
    """the stream is closed: the WEGA program has closed /dev/ud"""


class Link:
    def __init__(self):
        self.inp = sys.stdin.buffer.raw if hasattr(sys.stdin.buffer, 'raw') else sys.stdin.buffer
        self.out = sys.stdout.buffer

    def recv(self):
        b = os.read(0, 1)
        if not b:
            raise Stop()
        return b[0]

    def masked(self):		# MSKTRN: the byte without its parity bit
        return self.recv() & 0x7F

    def send(self, *bs):
        os.write(1, bytes(bs))


class Part8:
    def __init__(self, args):
        self.link = Link()
        self.drives = {n: getattr(args, 'drive%d' % n) for n in range(4) if getattr(args, 'drive%d' % n)}
        self.console = args.console
        self.date = args.date
        self.filler = b'\x00\x00'	# the two bytes behind the date (measured in MAME: 00 00; the system's memory)
        self.namlen = 0
        self.eprom = args.eprom
        self.eprom_type = {'e2708': 0, 'e2716': 1, 'e2732': 7, 'e2732a': 8, 'e2764': 9, 'e2764a': 10, 'e27128': 11,
                           'e27128a': 12, 'e27256': 13, 'e27512': 15}.get(args.eprom_type or 'e2764', 9)
        self.board = args.board
        self.ro = set(args.ro or [])
        self.short = args.short		# NAMLEN keeps its value from one request to the next (ISNAME sets it only for a good name)
        self.nambuf = b''

    # ---- the console of the 8-bit part
    def outas(self, text):
        if self.console:
            with open(self.console, 'ab') as f:
                f.write(text)

    def error(self, code):		# K.ACT.S ERROR: "File " NAME " Error XX" CR LF
        self.outas(b'File ' + self.nambuf[:self.namlen] + b' Error ' + b'%02X' % code + b'\r\n')

    # ---- the remote telegram
    def ack(self):
        self.link.send(ACK)

    def waitak(self):		# zero if ACK
        return self.link.masked()

    def filnam(self):
        """FILNAM: STX name ETX checksum; a wrong checksum: NAK and the name again. None: framing (the request ends)"""
        while True:
            c = self.link.masked()
            if c != STX:
                return ('frame', c)
            chk = 0
            name = bytearray()
            while True:
                c = self.link.masked()
                if c == ETX:
                    break
                if len(name) < 32:
                    name.append(c)
                chk ^= c
            if self.link.recv() == chk:
                return bytes(name)
            self.link.send(NAK)

    def isname(self, name):
        """ISNAME: an empty name, ';', blank, tab: 46H (invalid file name); NAMLEN is set only for a good name"""
        if not name or any(ch in b'; \t' for ch in name):
            return 0x46
        self.namlen = min(len(name), 32)
        return 0

    def ftype(self, flg):		# STRTRQ: P for binary+exec, B for binary, A otherwise
        if not flg & F_BINARY:
            return T_A
        return T_P if flg & F_EXEC else T_B

    def open_disk(self, name, output):
        """the drive of a name without a drive (*): input: the first drive where the file is; output: where it is, else the
        lowest drive with a disk. (disk, descriptor address or None) or an error code"""
        disks = [(n, udosfs.Disk(p)) for n, p in sorted(self.drives.items())]
        if not disks:
            return 0xC2		# NOT READY
        for n, d in disks:
            da = d.find(name.decode('latin-1'))
            if da is not None:
                return d, da
        if not output:
            return 0xC7		# FILE NOT FOUND
        return disks[-1][1], None	# measured: the last drive with a disk

    # ---- L: receive a UDOS file (putud)
    def load(self):
        self.ack()
        flg = self.link.recv()
        self.ack()
        name = self.filnam()
        if isinstance(name, tuple):
            return self.framer(name[1])
        self.nambuf = name
        flp = self.ftype(flg)
        if flg & F_FILDES:
            return self.load_desc(name, flg)
        err = self.isname(name)
        r = None
        if not err:
            r = self.open_disk(name, True)
            if isinstance(r, int):
                err = r
        if not err:
            disk, da = r
            rl, ftype, tail = RL, flp, None
            if da is not None:		# OPEN for OUTPUT of an existing file: its records go now; it takes the type and the
                od = disk.sector(da)	# record length asked for (measured in MAME), the date of creation stays
                tail = bytearray(od[0x28:0x80])
                disk.write_file(name.decode('latin-1'), ftype, rl, b'', 0, self.date, filler=self.filler,
                                desc_tail=bytes(tail))
                disk.save()
        if err:
            self.error(err)
            return self.cancel()
        self.ack()
        data = bytearray()
        while True:
            c = self.link.masked()
            if c == STX:
                hi = self.link.recv()
                lo = self.link.recv()
                n = hi * 256 + lo
                chk = 0
                blk = bytearray()
                for _ in range(n):
                    b = self.link.recv()
                    chk ^= b
                    blk.append(b)
                while self.link.masked() != ETX:	# LOKETX
                    pass
                if self.link.recv() != chk:
                    self.link.send(NAK)
                    continue
                data += blk
                self.ack()
            elif c in (CAN, ESC):
                break
        if flp & T_A:
            data.append(0xFF)		# CLOSEW: the end mark of an ASCII file
        err = self.write(disk, name, ftype, rl, data, da is not None, tail)
        if err:
            self.error(err)
        self.ack()			# CLOSE

    def write(self, disk, name, ftype, rl, data, existed, tail):
        """the file as WRTFIL and CLOSE leave it: records from a buffer preset with FFH (CLRBUF), the bytes of the last
        record (an ASCII end mark not counted), the segment of 28H (start 0, the length of everything written), HIGADR"""
        n = len(data)
        last = n % rl or (rl if n else 0)
        count = last - 1 if ftype & T_A else last
        if count < 0:
            count = 0
        padded = bytes(data) + b'\xff' * (-n % rl)
        if tail is None:
            tail = bytearray(b'\xff' * 0x58)
            tail[0:8] = bytes(8)
            tail[0x52:0x58] = bytes(6)
        tail = bytearray(tail)
        tail[0:8] = bytes(8)
        udosfs.put_le(tail, 2, n & 0xFFFF)		# FILSMT+2: the segment length
        udosfs.put_le(tail, 0x52, 0)			# LOWADR
        udosfs.put_le(tail, 0x54, (n - 1) & 0xFFFF)	# HIGADR
        udosfs.put_le(tail, 0x56, 0)			# STKSIZ
        try:
            disk.write_file(name.decode('latin-1'), ftype, rl, padded, count, self.date, filler=self.filler,
                            desc_tail=bytes(tail))
        except OSError as e:
            return e.errno or 0xD3
        disk.save()
        return 0

    def blocks(self):
        """the blocks of the data phase (WAISTX .. GIVACK): yields each good block; the caller answers it (ACK, or CAN to
        cancel); a block with a wrong checksum is answered NAK here; ends at CAN or ESC"""
        while True:
            c = self.link.masked()
            if c == STX:
                hi, lo = self.link.recv(), self.link.recv()
                blk = bytearray(self.link.recv() for _ in range(hi * 256 + lo))
                chk = 0
                for b in blk:
                    chk ^= b
                while self.link.masked() != ETX:
                    pass
                if self.link.recv() != chk:
                    self.link.send(NAK)
                    continue
                yield bytes(blk)
            elif c in (CAN, ESC):
                return

    def load_desc(self, name, flg):
        """L with a descriptor first (putud -f; K.ACT.S LOAD with FILDES, WRTDES): the first 80H bytes are 6 times MAGIC
        and the attributes; then the file is opened (LOAD1) with them and the data follow. The file gets the dates of the
        descriptor (WRTDES puts them into the system date for the time of the transfer) and its properties."""
        self.ack()
        head = bytearray()
        data = bytearray()
        opened = None
        for blk in self.blocks():
            if opened is None:
                take = blk[:0x80 - len(head)]
                head += take
                if len(head) < 0x80:
                    self.ack()
                    continue
                # the buffer of 80H bytes is full: WRTDES (the rest of this block is not stored: PUTBUF is left)
                if bytes(head[0:2]) != MAGIC:
                    self.outas(b'INVALID DESCRIPTOR\r\n')
                    return self.cancel()
                attr = bytes(head[12:12 + 0x74])
                rl = udosfs.le(attr, 3)
                if rl > BFRLEN:
                    self.outas(b'RECORD LENGTH TO LARGE\r\n')
                    return self.cancel()
                err = self.isname(name)
                r = None
                if not err:
                    r = self.open_disk(name, True)
                    if isinstance(r, int):
                        err = r
                if err:
                    self.error(err)
                    return self.cancel()
                opened = r + (attr,)
                self.ack()		# REDTRN
                continue
            data += blk
            self.ack()
        if opened is None:
            self.ack()			# CLOSE (nothing open)
            return
        disk, da, attr = opened
        rl = udosfs.le(attr, 3)
        n = len(data)
        last = n % rl or (rl if n else 0)
        date = attr[0x0C:0x12].decode('latin-1')
        try:
            disk.write_file(name.decode('latin-1'), attr[0], rl, bytes(data) + b'\xff' * (-n % rl), last, date,
                            filler=attr[0x12:0x14], props=attr[7], desc_tail=attr[0x1C:0x74])
            disk.save()
        except OSError as e:
            self.error(e.errno or 0xD3)
        self.ack()

    def touch(self, disk, da):
        """CLOSE of a file read: NDOS writes the descriptor back with the date of last change set to the system date
        (measured in MAME: getud changes it)"""
        d = bytearray(disk.sector(da))
        d[0x20:0x28] = self.date.encode('latin-1')[:6] + self.filler
        disk.put(da, d)
        disk.save()

    def cancel(self):		# CANCEL: CAN, wait for the answer, CLOSE, ACK
        self.link.send(CAN)
        self.waitak()
        self.ack()

    def framer(self, c):	# FRAMER: no STX before the name
        if c == CAN:
            self.ack()		# CANIN: close, ACK
        else:
            self.link.send(CAN)	# CANCL
            self.waitak()
            self.ack()

    # ---- S: send a UDOS file (getud)
    def send_file(self):
        self.ack()
        flg = self.link.recv()
        self.ack()
        name = self.filnam()
        if isinstance(name, tuple):
            return self.framer(name[1])
        self.nambuf = name
        flp = self.ftype(flg)
        err = self.isname(name)
        r = None
        if not err:
            r = self.open_disk(name, False)
            if isinstance(r, int):
                err = r
        if not err:
            disk, da = r
            d = disk.sector(da)
            if flg & F_FILDES:
                if not flg & F_BINARY:
                    err = 0x48		# OPN3: a descriptor transfer only of type binary
            elif (d[0x0C] & T_A) != (flp & T_A):
                err = 0x48		# INVALID FILE TYPE
        if err:
            self.error(err)
            self.link.send(CAN)	# CANCL
            self.waitak()
            self.ack()
            return
        self.ack()
        rl = udosfs.le(d, 0x0F)
        body = disk.read(d)		# the records, the last one cut to its byte count (count 0: the whole record)
        if udosfs.le(d, 0x16) == 0 and udosfs.le(d, 0x0D):
            body = b''.join([body])
        out = bytearray()
        if flg & F_FILDES:
            out += MAGIC * 6 + d[0x0C:0x80]
        if d[0x0C] & T_A:
            k = body.find(b'\xff')		# the end mark ends an ASCII file
            if k >= 0:
                body = body[:k]
        out += body
        i = 0
        while i < len(out):
            blk = out[i:i + XFERSZ]
            while True:
                chk = 0
                for b in blk:
                    chk ^= b
                self.link.send(STX, len(blk) >> 8, len(blk) & 0xFF, *blk)
                self.link.send(ETX, chk)
                a = self.waitak()
                if a == ACK:
                    break
                if a == NAK:
                    continue
                self.touch(disk, da)
                self.ack()		# CLOSE
                return
            i += XFERSZ
        self.link.send(EOT)		# DONRED
        self.touch(disk, da)
        self.ack()			# CLOSE

    # ---- P: the EPROM programmer (pburn)
    SIZES = {0: 0x400, 1: 0x800, 7: 0x1000, 8: 0x1000, 9: 0x2000, 10: 0x2000, 11: 0x4000, 12: 0x4000, 13: 0x8000,
             15: 0x10000}		# the EPROM types of pburn (pb.cmdline.c): 2708 ... 27512

    def type_ok(self, t):
        """K.P8000.S typ/ntyp: the types the card takes"""
        if self.board == 1:
            return t in (0, 1, 7, 8, 9)
        return t in (1, 7, 8, 9, 10, 11, 12, 13, 15)

    def chip(self):
        size = self.SIZES.get(self.eprom_type, 0x2000)
        if self.eprom and not os.path.exists(self.eprom):
            with open(self.eprom, 'wb') as f:
                f.write(b'\xff' * size)
        return bytearray(open(self.eprom, 'rb').read()) if self.eprom else bytearray(b'\xff' * size)

    def save_chip(self, c):
        if self.eprom:
            with open(self.eprom, 'wb') as f:
                f.write(c)

    @staticmethod
    def sdlc(ch, d, e):
        """K.P8000.S sdlc: one step of the CRC (the SDLC polynomial) over the byte ch, with d (high) and e (low)"""
        def rrca(a):
            return ((a >> 1) | (a << 7)) & 0xFF
        a = ch ^ d
        d = a
        a = rrca(rrca(rrca(rrca(a))))
        a = (a & 0x0F) ^ d
        d = a
        a = rrca(rrca(rrca(a)))
        saved = a
        e = (a & 0x1F) ^ e
        a = rrca(saved) & 0xF0
        e = a ^ e
        a = (saved & 0xE0) ^ d
        return e, a		# ld d,e; ld e,a

    def prom(self):
        rw = self.link.recv()
        ptype = self.link.recv()
        begadr = self.link.recv() << 8
        begadr |= self.link.recv()
        nbytes = self.link.recv() << 8
        nbytes |= self.link.recv()
        self.link.recv()		# Vpp
        pm = self.link.recv()		# program mode: 1 standard speed (read back), 0 high speed
        self.link.send(ord('q'))
        n = nbytes or 0x10000
        if rw == 2:			# write: the data first, 64 bytes and a 'q' each time
            buf = bytearray()
            while len(buf) < n:
                for _ in range(min(64, n - len(buf))):
                    buf.append(self.link.recv())
                self.link.send(ord('q'))
            err = self.check(ptype)
            if not err:
                c = self.chip()
                size = len(c)
                for i, b in enumerate(buf):
                    a = (begadr + i) % size
                    if b == 0xFF:
                        continue		# nwbyte: FF is not programmed
                    c[a] &= b
                    if c[a] != b:
                        err = 3
                        self.link.send(3)	# writeprom: the question
                        if self.link.recv() == ord('n'):
                            break
                        err = 0
                self.save_chip(c)
            if err != 3:
                self.link.send(err)
            if not err and pm == 1:		# standard speed: read back
                c = self.chip()
                for i, b in enumerate(buf):
                    if c[(begadr + i) % len(c)] != b:
                        err = 3
                        break
                self.link.send(err)
        elif rw == 1:			# read
            err = self.check(ptype)
            self.link.send(err)
            if not err:
                c = self.chip()
                data = bytes(c[(begadr + i) % len(c)] for i in range(n))
                for i in range(0, n, 64):
                    self.link.send(*data[i:i + 64])
                    self.link.recv()
        elif rw == 3:			# is it erased?
            err = self.check(ptype)
            if not err:
                c = self.chip()
                if any(c[(begadr + i) % len(c)] != 0xFF for i in range(n)):
                    err = 3
            self.link.send(err)
        elif rw == 4:			# CRC
            err = self.check(ptype)
            k = err
            d = e = 0xFF
            if not err:
                c = self.chip()
                for i in range(n):
                    ch = c[(begadr + i) % len(c)]
                    if ch != 0xFF:
                        k = 3
                    d, e = self.sdlc(ch, d, e)
            self.link.send(k)
            if not err:
                self.link.send(d, e)
        elif rw == 5:			# the board
            self.link.send(ord('2') if self.board == 2 else ord('1'))

    def check(self, ptype):
        """intppb: the type the card takes (1), a short circuit (2)"""
        if not self.type_ok(ptype):
            return 1
        if self.short:
            return 2
        return 0

    # ---- F V C R W: the floppies as blocks (K.BLK.S)
    GEOMETRY = {1: (77, 1, 26, 128), 2: (40, 2, 16, 256), 3: (40, 1, 16, 256), 4: (40, 2, 5, 1024),
                5: (80, 2, 16, 256), 6: (80, 2, 9, 512), 7: (80, 2, 5, 1024), 8: (40, 2, 9, 512), 9: (40, 1, 9, 512),
                10: (80, 1, 9, 512)}	# fd(4): tracks, sides, sectors per side, bytes per sector

    def geometry(self, minor):
        t = minor >> 4
        return self.GEOMETRY.get(t, self.GEOMETRY[6])		# TYP: the types above 10 as 6

    def drive_image(self, minor):
        """(path, size of the format) or an error: C2 when the drive has no disk"""
        n = minor & 3
        p = self.drives.get(n)
        tr, sd, ns, sl = self.geometry(minor)
        if not p or not os.path.exists(p):
            return 0xC2
        return p, tr * sd * ns * sl

    def read_disk(self, minor):
        r = self.drive_image(minor)
        if isinstance(r, int):
            return r
        p, size = r
        data = open(p, 'rb').read()
        if len(data) != size:
            return 0xC4		# not formatted in this format: sector error
        return data

    def write_disk(self, minor, data):
        r = self.drive_image(minor)
        if isinstance(r, int):
            return r
        if (minor & 3) in self.ro:
            return 0xC3
        with open(r[0], 'wb') as f:
            f.write(data)
        return 0

    def blkfo(self, req):
        """BLKFO: F formats and verifies, V verifies, C formats the destination, copies the source onto it and verifies;
        the answer ACK or the error code (of the source with bit 5)"""
        smin = self.link.recv() if req == 'C' else None
        dmin = self.link.recv()
        tr, sd, ns, sl = self.geometry(dmin)
        size = tr * sd * ns * sl
        err = 0
        if req in 'FC':
            r = self.drive_image(dmin)
            err = r if isinstance(r, int) else self.write_disk(dmin, b'\xe5' * size)
        if not err and req == 'C':
            src = self.read_disk(smin)
            if isinstance(src, int):
                err = src | 0x20
            else:
                err = self.write_disk(dmin, src)
        if not err:
            d = self.read_disk(dmin)
            if isinstance(d, int):
                err = d
        self.link.send(err or ACK)

    def block_header(self):
        """KOPF: the minor number, the block number (512 bytes a block), the length; the offset in the image"""
        minor = self.link.recv()
        blk = self.link.recv() << 8
        blk |= self.link.recv()
        n = self.link.recv() << 8
        n |= self.link.recv()
        return minor, blk * 512, n

    def block_read(self):
        minor, off, n = self.block_header()
        d = self.read_disk(minor)
        if isinstance(d, int):
            self.link.send(d)
            return
        self.link.send(ACK)
        self.link.send(*d[off:off + n].ljust(n, b'\0'))
        self.link.recv()		# WATACK

    def block_write(self):
        minor, off, n = self.block_header()
        data = bytes(self.link.recv() for _ in range(n))
        d = self.read_disk(minor)
        if isinstance(d, int):
            self.link.send(d)
            return
        d = bytearray(d)
        d[off:off + n] = data
        err = self.write_disk(minor, bytes(d))
        self.link.send(err or ACK)

    def run(self):
        try:
            while True:
                c = self.link.recv()
                if c == ord('L'):
                    self.load()
                elif c == ord('S'):
                    self.send_file()
                elif c == ord('P'):
                    self.prom()
                elif c in b'FVC':
                    self.blkfo(chr(c))
                elif c == ord('R'):
                    self.block_read()
                elif c == ord('W'):
                    self.block_write()
                # any other byte: no request (RSTART is reset), dropped
        except Stop:
            return 0


def main():
    ap = argparse.ArgumentParser()
    for n in range(4):
        ap.add_argument('--drive%d' % n)
    ap.add_argument('--console')
    ap.add_argument('--date', default='890509')
    ap.add_argument('--eprom')
    ap.add_argument('--eprom-type')
    ap.add_argument('--board', type=int, default=2)
    ap.add_argument('--short', action='store_true')
    ap.add_argument('--ro', type=int, action='append')
    return Part8(ap.parse_args()).run()


if __name__ == '__main__':
    sys.exit(main())
