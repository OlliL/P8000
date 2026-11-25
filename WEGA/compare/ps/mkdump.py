#!/usr/bin/env python3
"""mkdump.py FILE: a small crash dump for `ps -k FILE` (the corpus of ps).

The layout that kva.c and kpa.c of /bin/ps read: the first 1024 bytes hold the MMU table (the segment descriptors at
256: base (clicks of 256 bytes), limit, attributes, 4 bytes each); a kernel address SEG:OFF is at
(base << 8) + 1024 + OFF in the file, a physical address PA at PA + 1024. The symbols of /wega: _Nproc 01000034,
_swplo 010005c0 (segment 1), _proc 0300df58 (segment 3). The proc table (struct proc, 36 bytes) and the user blocks
(struct user, 0x572 bytes, at (p_addr << 8) + 1024) of a few processes:
  pid 0  swapper, in core
  pid 1  init, in core, no tty, its stack without arguments
  pid 30 sh on the console (dev 8/1), in core, uid 100, its stack with the arguments "-sh"
  pid 31 a process on tty2 (dev 8/2), in core, its stack with the arguments "cat" "x=y"
  pid 40 a zombie (struct xproc)
  pid 50 swapped out (not in core: -k gives a zeroed user block)
  one empty slot
"""
import struct
import sys

SEG1, SEG3 = 0x000, 0x200          # bases of the kernel segments 1 and 3 (clicks)
NPROC = 7
UBASE = 0x400                      # p_addr of the first user block (clicks)
USZ = 0x572
PSIZE = 0x20                       # p_size (clicks): 8 KB


def put(buf, at, data):
    if len(buf) < at + len(data):
        buf.extend(b'\0' * (at + len(data) - len(buf)))
    buf[at:at + len(data)] = data


def kv(seg, base, off):
    return (base << 8) + 1024 + off


def proc(stat, flag, pri, cpu, nice, uid, pgrp, pid, ppid, addr, size, wchan):
    return struct.pack('>bbbbbbiHHHHHHIIIH', stat, flag, pri, 0, cpu, nice, 0, uid, pgrp, pid, ppid, addr, size,
                       wchan, 0, 0, 0)


def xproc(stat, flag, pri, cpu, nice, uid, pgrp, pid, ppid, utime, stime):
    return struct.pack('>bbbbbbiHHHHHii', stat, flag, pri, 0, cpu, nice, 0, uid, pgrp, pid, ppid, 0, utime,
                       stime) + b'\0' * 4


def user(comm, ttyp, ttyd, utime, stime, start, tsize=4, dsize=4, ssize=4):
    u = bytearray(USZ)
    put(u, 0xD8, struct.pack('>HHH', tsize, dsize, ssize))
    put(u, 0x156, struct.pack('>ii', utime, stime))
    put(u, 0x1DE, struct.pack('>IH', ttyp, ttyd))
    put(u, 0x4AA, comm.encode()[:14])
    put(u, 0x4BC, struct.pack('>i', start))
    return bytes(u)


def main():
    out = bytearray(1024)
    for seg, base in ((1, SEG1), (3, SEG3)):
        put(out, 256 + 4 * seg, struct.pack('>HBB', base, 0xff, 0))
    put(out, kv(1, SEG1, 0x34), struct.pack('>H', NPROC))
    put(out, kv(1, SEG1, 0x5c0), struct.pack('>i', 0x1000))
    now = 425000000
    procs = [
        proc(1, 3, 0, 0, 20, 0, 0, 0, 0, UBASE, PSIZE, 0x0300a000),
        proc(1, 1, 30, 0, 20, 0, 0, 1, 0, UBASE + PSIZE, PSIZE, 0x0300b010),
        proc(3, 1, 60, 12, 20, 100, 30, 30, 1, UBASE + 2 * PSIZE, PSIZE, 0),
        proc(1, 1, 40, 3, 24, 100, 30, 31, 30, UBASE + 3 * PSIZE, PSIZE, 0x0100c000),
        xproc(5, 0, 50, 2, 20, 100, 30, 40, 30, 125, 61),
        proc(1, 0, 70, 0, 20, 2, 50, 50, 1, 0x20, PSIZE, 0x0300d000),
        b'\0' * 36,
    ]
    for i, p in enumerate(procs):
        put(out, kv(3, SEG3, 0xdf58) + 36 * i, p)
    users = [
        user('swapper', 0, 0, 0, 0, now - 90000),
        user('init', 0, 0, 30, 15, now - 90000),
        user('sh', 0x0300e000, 0x0801, 600, 300, now - 600),
        user('cat', 0x0300e100, 0x0802, 4000, 1000, now - 30),
    ]
    for i, u in enumerate(users):
        at = ((UBASE + i * PSIZE) << 8) + 1024
        put(out, at, u)
        top = at + (PSIZE << 8) - 512
        if i == 2:
            # the last word of the stack: a pointer (the two bytes of the segment word swapped) to argv at 0x0000ff00
            put(out, top + 508, struct.pack('>I', 0x0000ff00))
        if i == 3:
            put(out, top + 508, struct.pack('>I', 0x0000ff10))
        if i == 1:
            # no pointer: the arguments are looked for in the last 512 bytes (a -1 word, then the strings)
            put(out, top + 480, struct.pack('>h', -1) + b'/etc/init\0')
    open(sys.argv[1], 'wb').write(bytes(out))


if __name__ == '__main__':
    main()
