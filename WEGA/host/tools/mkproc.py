#!/usr/bin/env python3
"""mkproc.py PROC ...: the process table of the kernel as ps reads it (device: kmem image:km.img, mem image:mem.img, swap image:swap.img):
writes km.img, mem.img and swap.img into the working directory. PROC is PID:PPID:UID:TTY:STATE:COMMAND[:START[:UTIME]]

  TTY    co (the console), a number N (tty N), or ? (none)
  STATE  R running, S sleeping, Z zombie, T stopped, I being made
  START  seconds since 1970 (default 599990000), UTIME the user time in seconds (default 0)

What ps (/bin/ps, 3.2) reads: the number of table entries _Nproc and the symbols _proc and _swplo of /wega (the segmented addresses
0x01000034, 0x0300df58, 0x010005c0) in /dev/kmem; for a process in core (flag SLOAD) the user structure of 1394 bytes in /dev/mem at
p_addr * 256 (u_comm 1194, u_ttyd 482, u_ttyp 478, u_start 1212, u_utime 342, u_stime 346: measured with the compiler of the kernel,
sccvk, from the headers of src/head/sys), the layout of struct proc (36 bytes) as proc.h. Also _Nmount (0x01000032, 8) for df.
The process group of a process is its own when its name begins with - (a login shell) or its parent is 0, else its parent's: ps -a
leaves out the leaders. All numbers big endian (Z8000). ps -f shows [ COMMAND ] for the name of the process: it does not look for the arguments."""
import struct
import sys

STATE = {'R': 3, 'S': 1, 'Z': 5, 'T': 6, 'I': 4}


def main(args):
    km = open('km.img', 'wb')
    km.truncate(0x03010000)
    km.seek(0x01000032)
    km.write(struct.pack('>H', 8))
    km.seek(0x01000034)
    km.write(struct.pack('>H', len(args) + 2))
    mem = bytearray(0x100000)
    km.seek(0x0300df58)
    pgrp = {}
    for a in args:	# the process group: a login shell (name begins with -) and init lead their own, the others belong to their parent's
        f = a.split(':')
        pgrp[int(f[0])] = int(f[0]) if f[5].startswith('-') or int(f[1]) == 0 else pgrp.get(int(f[1]), int(f[1]))
    for n, a in enumerate(args):
        f = a.split(':')
        pid, ppid, uid = int(f[0]), int(f[1]), int(f[2])
        tty = 0 if f[3] == '?' else (8 << 8 | 1 if f[3] == 'co' else 8 << 8 | int(f[3]))
        addr = 0x20 + n * 8
        b = bytearray(36)
        b[0], b[1] = STATE[f[4]], 1
        struct.pack_into('>hhhhhh', b, 10, uid, pgrp[pid], pid, ppid, addr, 4)
        u = bytearray(1394)
        u[1194:1208] = f[5].encode().ljust(14, b'\0')[:14]
        struct.pack_into('>H', u, 482, tty)
        struct.pack_into('>i', u, 478, 0x01000000 if tty else 0)
        struct.pack_into('>i', u, 1212, int(f[6]) if len(f) > 6 else 599990000)
        struct.pack_into('>i', u, 342, int(f[7]) if len(f) > 7 else 0)
        mem[addr << 8:(addr << 8) + 1394] = u
        km.write(b)
    km.close()
    open('mem.img', 'wb').write(mem)
    open('swap.img', 'wb').truncate(0x100000)


if __name__ == '__main__':
    main(sys.argv[1:])
