#!/usr/bin/env python3
"""ud8.py - the model of the 8-bit part of the P8000 behind /dev/ud (increment 117).

z8k-emu starts it for an open of /dev/ud (Z8K_DEVTAB: model ud proc:...); its stdin and stdout are the byte stream of the
coupling: what the WEGA program writes to /dev/ud arrives here, what is written here the program reads.

  ud8.py [--drive0 IMAGE] [--drive1 IMAGE] ... [--console FILE] [--date YYMMDD]

It is the coupling software of the 8-bit part (UDOS/WEGA/K.*.S beside WEGA/, koppeldoc) transcribed: it waits for a
request letter and serves it (an unknown letter is dropped, as the request dispatch of K.NEU.S does):
  L  LOAD: receive a UDOS file (putud)        K.ACT.S LOAD
  S  SEND: send a UDOS file (getud)           K.ACT.S SEND
The UDOS file system (NDOS) is host/tools/udosfs.py on the image files of the drives. What the 8-bit part prints on its
console (the errors: "File NAME Error XX") goes to the file of --console. The system date of UDOS is --date (default
890509, the date of the directory of the WEGA start disk; UDOS asks for it when it starts).

Not measured, the model's own choice (host/increments/117-device-models.md): a new file without a drive (putud never
sends one) is made on the lowest drive that has a disk; NDOS's choice of free sectors is the writer's rule of udosfs.py.
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
        self.namlen = 0		# NAMLEN keeps its value from one request to the next (ISNAME sets it only for a good name)
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
        return disks[0][1], None

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
            if da is not None:		# OPEN for OUTPUT of an existing file: its records go now, its attributes stay
                od = disk.sector(da)
                rl, ftype = udosfs.le(od, 0x0F), od[0x0C]
                tail = bytearray(od[0x28:0x80])
                disk.write_file(name.decode('latin-1'), ftype, rl, b'', 0, self.date, desc_tail=bytes(tail))
                disk.save()
                if (ftype & T_A) != (flp & T_A):
                    err = 0x48		# INVALID FILE TYPE (OPN3)
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
            disk.write_file(name.decode('latin-1'), ftype, rl, padded, count, self.date, desc_tail=bytes(tail))
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
                self.ack()		# CLOSE
                return
            i += XFERSZ
        self.link.send(EOT)		# DONRED
        self.ack()			# CLOSE

    def run(self):
        try:
            while True:
                c = self.link.recv()
                if c == ord('L'):
                    self.load()
                elif c == ord('S'):
                    self.send_file()
                # any other byte: no request (RSTART is reset), dropped
        except Stop:
            return 0


def main():
    ap = argparse.ArgumentParser()
    for n in range(4):
        ap.add_argument('--drive%d' % n)
    ap.add_argument('--console')
    ap.add_argument('--date', default='890509')
    return Part8(ap.parse_args()).run()


if __name__ == '__main__':
    sys.exit(main())
