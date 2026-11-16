#!/usr/bin/env python3
"""wegafs.py - the WEGA file system (System III) in an image: a floppy, a partition of a hard disk.

  wegafs.py info IMAGE               the super block: size, i-node blocks, free blocks and i-nodes, name, pack
  wegafs.py ls IMAGE [PATH]          a directory (or one file) as ls -l: mode, links, uid, gid, size, time, name
  wegafs.py cat IMAGE PATH           the data of a file on stdout
  wegafs.py extract IMAGE DIR        the whole tree into DIR (made): directories, files, links (a second name of an
                                     i-node is a hard link again), modes, times; the owners and what the host cannot
                                     make (device files) are listed in DIR.owners: uid gid mode path
  wegafs.py scrub IMAGE              zero the bytes of the super block behind the structure (s_mach and on): what a program leaves there
                                     is leftover memory (restor: digits of its process number), not part of the file system
  wegafs.py format IMAGE BLOCKS [TIME]   an empty file system of BLOCKS blocks, byte for byte as WEGA's /etc/mkfs
                                     IMAGE BLOCKS makes it (format_fs; TIME the clock, seconds since 1970)
  wegafs.py put IMAGE PATH FILE [MODE [UID GID [TIME]]]   a file into the file system (made or replaced; MODE octal,
                                     default 644; its directory must exist)
  wegafs.py mkdir IMAGE PATH [MODE [UID GID [TIME]]]      a directory (with . and ..; default 755)
  put and mkdir take the free blocks from the free list and the lowest free i-node: a valid file system, not the
  blocks and i-nodes the kernel would choose.

The format (src/head/sys/filsys.h, ino.h, dir.h, param.h; read from the WEGA 3.1 disks with mame/tools/wegafs.py):
- Blocks of 512 bytes (BSIZE); numbers are big-endian (Z8000); the file system starts at block 0 of the image.
- Block 0: the boot block. Block 1: the super block: s_isize (2) the blocks of the i-list, s_fsize (4) the blocks of
  the file system, s_nfree (2) and s_free[50] (4 each) the free blocks at hand, s_ninode (2) and s_inode[100] (2 each)
  the free i-nodes at hand, s_flock, s_ilock, s_fmod, s_ronly (1 each), s_time (4), s_tfree (4), s_tinode (2), s_m,
  s_n (2 each), s_fname[6], s_fpack[6].
- Blocks 2 .. s_isize+1: the i-nodes, 64 bytes each, numbered from 1 (the root directory is i-node 2): di_mode (2),
  di_nlink (2), di_uid (2), di_gid (2), di_size (4), di_addr (40: 13 block numbers of 3 bytes, 10 direct, one single,
  double and triple indirect), di_atime, di_mtime, di_ctime (4 each). A device file has its device number in the first
  address.
- An indirect block: 128 block numbers of 4 bytes (NINDIR); block number 0 is a hole.
- A directory: entries of 16 bytes, the i-node number (2, 0: free) and the name (14, DIRSIZ, filled with NUL).
- The free blocks: s_free[0 .. s_nfree-1] in the super block, the last one taken first; s_free[0] is the block that
  holds the next 50 (a block: df_nfree (2), df_free[50] (4 each)), 0 ends the list.
WEGA's /etc/mkfs (version 1.5 of 12/12/83, measured under z8k-emu: no source): `mkfs FILE BLOCKS` makes BLOCKS / 25
i-node blocks (s_isize = BLOCKS / 25 + 2; it prints "isize = " s_isize * 8), m/n 1 72 always (the interleave of the free
list, s_m, s_n); "BLOCKS:INODES" is not understood. Otherwise as System III mkfs.c (tmp/sys3): the free list from the
last block down in groups of n, i-node 1 (the bad blocks, none) and i-node 2 (the root directory, mode 40777, uid and
gid 0, with . and ..) made with the time of the clock; the file ends with the last block written. s_tfree counts
the block of the root directory as free (one too high; System III mkfs.c does not)."""
import os
import struct
import sys
import time

BSIZE = 512
ISIZE = 64            # bytes of an i-node
INOPB = BSIZE // ISIZE
NINDIR = BSIZE // 4
DIRENT = 16
IFMT, IFDIR, IFCHR, IFBLK, IFREG, IFIFO = 0o170000, 0o040000, 0o020000, 0o060000, 0o100000, 0o010000
ROOTINO = 2


def be(b, o, n):
    return int.from_bytes(b[o:o + n], 'big')


class Inode:
    def __init__(self, num, raw):
        self.num = num
        self.mode, self.nlink, self.uid, self.gid, self.size = struct.unpack('>HhHHI', raw[:12])
        self.addr = [be(raw, 12 + 3 * k, 3) for k in range(13)]
        self.atime, self.mtime, self.ctime = struct.unpack('>III', raw[52:64])

    def kind(self):
        return self.mode & IFMT


class Image:
    def __init__(self, path):
        self.data = bytearray(open(path, 'rb').read())
        sb = self.block(1)
        self.isize = be(sb, 0, 2)
        self.fsize = be(sb, 2, 4)
        if not (0 < self.isize < self.fsize) or self.inode(ROOTINO).kind() != IFDIR:
            raise ValueError('%s: no WEGA file system' % path)

    def block(self, n):
        return bytes(self.data[n * BSIZE:(n + 1) * BSIZE]).ljust(BSIZE, b'\0')

    def superblock(self):
        """the fields of the super block (filsys.h) by name"""
        sb = self.block(1)
        o = 6 + 2 + 50 * 4
        ninode = be(sb, o, 2)
        o += 2 + 100 * 2
        return {'isize': self.isize, 'fsize': self.fsize, 'nfree': be(sb, 6, 2), 'ninode': ninode,
                'flock': sb[o], 'ilock': sb[o + 1], 'fmod': sb[o + 2], 'ronly': sb[o + 3], 'time': be(sb, o + 4, 4),
                'tfree': be(sb, o + 8, 4), 'tinode': be(sb, o + 12, 2), 'm': be(sb, o + 14, 2),
                'n': be(sb, o + 16, 2), 'fname': sb[o + 18:o + 24].split(b'\0')[0].decode('latin-1'),
                'fpack': sb[o + 24:o + 30].split(b'\0')[0].decode('latin-1')}

    def inode(self, num):
        b = self.block(2 + (num - 1) // INOPB)
        o = ((num - 1) % INOPB) * ISIZE
        return Inode(num, b[o:o + ISIZE])

    def blocks(self, ino):
        """the block numbers of the data of a file, as many as its size needs (0: a hole)"""
        n = (ino.size + BSIZE - 1) // BSIZE
        out = ino.addr[:10][:n]

        def ind(blk, level):
            for k in range(NINDIR):
                if len(out) >= n:
                    return
                p = be(self.block(blk), 4 * k, 4) if blk else 0
                if level == 1:
                    out.append(p)
                else:
                    ind(p, level - 1)
        for level, a in ((1, ino.addr[10]), (2, ino.addr[11]), (3, ino.addr[12])):
            if len(out) < n:
                ind(a, level)
        return out[:n]

    def read(self, ino):
        if ino.kind() in (IFCHR, IFBLK):
            return b''
        d = b''.join(self.block(b) if b else bytes(BSIZE) for b in self.blocks(ino))
        return d[:ino.size]

    def directory(self, ino):
        """the entries of a directory: (i-node number, name), the free ones left out"""
        d = self.read(ino)
        out = []
        for o in range(0, len(d) - DIRENT + 1, DIRENT):
            num = be(d, o, 2)
            if num:
                out.append((num, d[o + 2:o + DIRENT].split(b'\0')[0].decode('latin-1')))
        return out

    def lookup(self, path):
        ino = self.inode(ROOTINO)
        for part in [p for p in path.split('/') if p]:
            for num, name in self.directory(ino):
                if name == part:
                    ino = self.inode(num)
                    break
            else:
                raise KeyError(path)
        return ino

    def walk(self, ino=None, path=''):
        """(path, i-node) of the tree, a directory before what is in it"""
        ino = ino or self.inode(ROOTINO)
        yield path or '/', ino
        if ino.kind() == IFDIR:
            for num, name in self.directory(ino):
                if name not in ('.', '..'):
                    yield from self.walk(self.inode(num), path + '/' + name)


def put_be(b, o, n, v):
    b[o:o + n] = (v & ((1 << 8 * n) - 1)).to_bytes(n, 'big')


SB_TIME = 6 + 2 + 50 * 4 + 2 + 100 * 2 + 4	# offsets in the super block (filsys.h)
SB_TFREE, SB_TINODE, SB_M, SB_N = SB_TIME + 4, SB_TIME + 8, SB_TIME + 10, SB_TIME + 12


class Disk(Image):
    """an image to change: the super block in sb, written back by save"""
    def __init__(self, path):
        Image.__init__(self, path)
        self.path = path
        self.sb = bytearray(self.block(1))

    def put(self, n, data):
        end = (n + 1) * BSIZE
        if len(self.data) < end:
            self.data.extend(bytes(end - len(self.data)))
        self.data[n * BSIZE:end] = bytes(data).ljust(BSIZE, b'\0')

    def save(self):
        self.put(1, self.sb)
        with open(self.path, 'wb') as f:
            f.write(self.data)

    def alloc(self):
        """a free block (mkfs.c alloc: the last of s_free; the next group read from the block in s_free[0])"""
        nfree = be(self.sb, 6, 2) - 1
        b = be(self.sb, 8 + 4 * nfree, 4)
        if b == 0:
            raise ValueError('out of free space')
        put_be(self.sb, SB_TFREE, 4, be(self.sb, SB_TFREE, 4) - 1)
        if nfree <= 0:
            fb = self.block(b)
            nfree = be(fb, 0, 2)
            self.sb[8:8 + 200] = fb[2:202]
        put_be(self.sb, 6, 2, nfree)
        return b

    def write_inode(self, num, mode, nlink, uid, gid, size, addr, t):
        b = bytearray(self.block(2 + (num - 1) // INOPB))
        o = ((num - 1) % INOPB) * ISIZE
        raw = struct.pack('>HhHHI', mode, nlink, uid, gid, size)
        raw += b''.join(a.to_bytes(3, 'big') for a in (list(addr) + [0] * 13)[:13]) + b'\0'
        raw += struct.pack('>III', t, t, t)
        b[o:o + ISIZE] = raw
        self.put(2 + (num - 1) // INOPB, b)

    def ialloc(self):
        for num in range(1, (self.isize - 2) * INOPB + 1):
            if self.inode(num).mode == 0:
                put_be(self.sb, SB_TINODE, 2, be(self.sb, SB_TINODE, 2) - 1)
                return num
        raise ValueError('out of i-nodes')

    def write_data(self, data):
        """data into new blocks: the 13 addresses of the i-node (direct, single and double indirect)"""
        blks = []
        for o in range(0, len(data), BSIZE):
            b = self.alloc()
            self.put(b, data[o:o + BSIZE])
            blks.append(b)
        addr = blks[:10]
        rest = blks[10:]
        if rest:
            ib = self.alloc()
            self.put(ib, b''.join(x.to_bytes(4, 'big') for x in rest[:NINDIR]))
            addr.append(ib)
            rest = rest[NINDIR:]
        if rest:
            groups = []
            while rest:
                g = self.alloc()
                self.put(g, b''.join(x.to_bytes(4, 'big') for x in rest[:NINDIR]))
                groups.append(g)
                rest = rest[NINDIR:]
            ib2 = self.alloc()
            self.put(ib2, b''.join(x.to_bytes(4, 'big') for x in groups))
            addr.append(ib2)
        return addr

    def free_data(self, ino):
        """the blocks of a file back onto the free list (mkfs.c bfree)"""
        for b in [b for b in self.blocks(ino) if b] + [a for a in ino.addr[10:12] if a]:
            nfree = be(self.sb, 6, 2)
            if nfree >= 50:
                self.put(b, self.sb[6:8] + self.sb[8:208])
                nfree = 0
            put_be(self.sb, 8 + 4 * nfree, 4, b)
            put_be(self.sb, 6, 2, nfree + 1)
            put_be(self.sb, SB_TFREE, 4, be(self.sb, SB_TFREE, 4) + 1)

    def add_entry(self, dino, num, name):
        """a directory entry: into a free slot, else at the end (the directory rewritten)"""
        d = bytearray(self.read(dino))
        ent = num.to_bytes(2, 'big') + name.encode('latin-1')[:14].ljust(14, b'\0')
        for o in range(0, len(d), DIRENT):
            if be(d, o, 2) == 0:
                d[o:o + DIRENT] = ent
                break
        else:
            d += ent
        self.free_data(dino)
        self.write_inode(dino.num, dino.mode, dino.nlink, dino.uid, dino.gid, len(d), self.write_data(bytes(d)),
                         dino.mtime)

    def make(self, path, mode, uid, gid, t, data=b''):
        """a file or a directory at path (a file of that name is replaced, keeping its i-node)"""
        parent, name = path.rstrip('/').rsplit('/', 1)
        dino = self.lookup(parent or '/')
        old = [n for n, nm in self.directory(dino) if nm == name]
        if mode & IFMT == IFDIR:
            num = self.ialloc()
            self.write_inode(num, mode, 2, uid, gid, 0, [], t)
            body = num.to_bytes(2, 'big') + b'.'.ljust(14, b'\0') + dino.num.to_bytes(2, 'big') + b'..'.ljust(14, b'\0')
            self.write_inode(num, mode, 2, uid, gid, len(body), self.write_data(body), t)
            self.add_entry(dino, num, name)
            dino = self.inode(dino.num)
            self.write_inode(dino.num, dino.mode, dino.nlink + 1, dino.uid, dino.gid, dino.size, dino.addr, dino.mtime)
            return num
        if old:
            num = old[0]
            ino = self.inode(num)
            self.free_data(ino)
            self.write_inode(num, mode, ino.nlink, uid, gid, len(data), self.write_data(data), t)
            return num
        num = self.ialloc()
        self.write_inode(num, mode, 1, uid, gid, len(data), self.write_data(data), t)
        self.add_entry(dino, num, name)
        return num


def format_fs(path, nblocks, t):
    """an empty file system as WEGA's /etc/mkfs PATH NBLOCKS makes it (System III mkfs.c with WEGA's sizes)"""
    img = {}
    sb = bytearray(BSIZE)
    isize = nblocks // 25 + 2
    put_be(sb, 0, 2, isize)
    put_be(sb, 2, 4, nblocks)
    put_be(sb, SB_M, 2, 1)
    put_be(sb, SB_N, 2, 72)
    fm, fn = 1, 72
    for n in range(2, isize):
        img[n] = bytes(BSIZE)
    put_be(sb, SB_TINODE, 2, (isize - 2) * INOPB)

    def bfree(b):
        put_be(sb, SB_TFREE, 4, be(sb, SB_TFREE, 4) + 1)
        nfree = be(sb, 6, 2)
        if nfree >= 50:
            img[b] = bytes(sb[6:8] + sb[8:208]).ljust(BSIZE, b'\0')
            nfree = 0
        put_be(sb, 8 + 4 * nfree, 4, b)
        put_be(sb, 6, 2, nfree + 1)

    def alloc():	# WEGA's mkfs does not count the block it takes (s_tfree one too high: measured)
        nfree = be(sb, 6, 2) - 1
        b = be(sb, 8 + 4 * nfree, 4)
        if nfree <= 0:
            fb = img[b]
            nfree = be(fb, 0, 2)
            sb[8:208] = fb[2:202]
        put_be(sb, 6, 2, nfree)
        return b

    def iput(num, mode, nlink, size, addr):
        put_be(sb, SB_TINODE, 2, be(sb, SB_TINODE, 2) - 1)
        n = 2 + (num - 1) // INOPB
        b = bytearray(img[n])
        o = ((num - 1) % INOPB) * ISIZE
        raw = struct.pack('>HhHHI', mode, nlink, 0, 0, size)
        raw += b''.join(a.to_bytes(3, 'big') for a in (list(addr) + [0] * 13)[:13]) + b'\0'
        b[o:o + ISIZE] = raw + struct.pack('>III', t, t, t)
        img[n] = bytes(b)

    # bflist: the free list, interleaved by m and n; i-node 1 holds the bad blocks (none)
    flg = [0] * fn
    adr = []
    i = 0
    for j in range(fn):
        while flg[i]:
            i = (i + 1) % fn
        adr.append(i + 1)
        flg[i] += 1
        i = (i + fm) % fn
    bfree(0)
    put_be(sb, SB_TFREE, 4, 0)
    d = nblocks - 1
    while d % fn:
        d += 1
    while d > 0:
        for a in adr:
            f = d - a
            if isize <= f < nblocks:
                bfree(f)
        d -= fn
    iput(1, IFREG, 0, 0, [])
    # cfile: the root directory, "d--777 0 0 $"
    db = (2).to_bytes(2, 'big') + b'.'.ljust(14, b'\0') + (2).to_bytes(2, 'big') + b'..'.ljust(14, b'\0')
    b = alloc()
    img[b] = db.ljust(BSIZE, b'\0')
    iput(2, IFDIR | 0o777, 2, 32, [b])
    put_be(sb, SB_TIME, 4, t)
    img[1] = bytes(sb)
    out = bytearray((max(img) + 1) * BSIZE)
    for n, data in img.items():
        out[n * BSIZE:(n + 1) * BSIZE] = data
    with open(path, 'wb') as f:
        f.write(out)


def mode_string(mode):
    s = {IFDIR: 'd', IFCHR: 'c', IFBLK: 'b', IFREG: '-', IFIFO: 'p'}.get(mode & IFMT, '?')
    for shift in (6, 3, 0):
        bits = (mode >> shift) & 7
        s += ('r' if bits & 4 else '-') + ('w' if bits & 2 else '-') + ('x' if bits & 1 else '-')
    s = list(s)
    for bit, i, on in ((0o4000, 3, 's'), (0o2000, 6, 's'), (0o1000, 9, 't')):
        if mode & bit:
            s[i] = on if s[i] == 'x' else on.upper()
    return ''.join(s)


def ls_line(ino, name):
    size = '%d,%d' % (ino.addr[0] >> 8 & 0xFF, ino.addr[0] & 0xFF) if ino.kind() in (IFCHR, IFBLK) else ino.size
    return '%s %3d %5d %5d %8s %s %s' % (mode_string(ino.mode), ino.nlink, ino.uid, ino.gid, size,
                                         time.strftime('%Y-%m-%d %H:%M', time.gmtime(ino.mtime)), name)


def extract(img, out):
    """the tree into the directory out; DIR.owners: uid gid mode path (the host user owns the files it makes)"""
    os.makedirs(out, exist_ok=True)
    first = {}
    owners = []
    dirs = []
    for path, ino in img.walk():
        p = os.path.join(out, path.lstrip('/'))
        owners.append('%d %d %o %s' % (ino.uid, ino.gid, ino.mode, path))
        if ino.kind() == IFDIR:
            os.makedirs(p, exist_ok=True)
            dirs.append((p, ino))
            continue
        if ino.num in first:		# a second name of the i-node
            os.link(first[ino.num], p)
            continue
        if ino.kind() == IFREG:
            with open(p, 'wb') as f:
                f.write(img.read(ino))
        elif ino.kind() == IFIFO:
            os.mkfifo(p)
        else:				# a device file: only listed (the host user cannot make it)
            continue
        os.chmod(p, ino.mode & 0o7777)
        os.utime(p, (ino.atime, ino.mtime))
        first[ino.num] = p
    for p, ino in reversed(dirs):	# the directories last: writing into them changed their times
        os.chmod(p, ino.mode & 0o7777)
        os.utime(p, (ino.atime, ino.mtime))
    with open(out.rstrip('/') + '.owners', 'w') as f:
        f.write('\n'.join(owners) + '\n')


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    if sys.argv[1] == 'format' and len(sys.argv) > 3:
        format_fs(sys.argv[2], int(sys.argv[3]), int(sys.argv[4]) if len(sys.argv) > 4 else int(time.time()))
        return 0
    if sys.argv[1] in ('put', 'mkdir') and len(sys.argv) > 3:
        disk = Disk(sys.argv[2])
        rest = sys.argv[4:] if sys.argv[1] == 'put' else sys.argv[3:]
        rest = rest[1:] if sys.argv[1] == 'put' else rest[1:]
        mode = int(rest[0], 8) if rest else (0o644 if sys.argv[1] == 'put' else 0o755)
        uid, gid = (int(rest[1]), int(rest[2])) if len(rest) > 2 else (0, 0)
        t = int(rest[3]) if len(rest) > 3 else int(time.time())
        if sys.argv[1] == 'put':
            disk.make(sys.argv[3], IFREG | mode, uid, gid, t, open(sys.argv[4], 'rb').read())
        else:
            disk.make(sys.argv[3], IFDIR | mode, uid, gid, t)
        disk.save()
        return 0
    if sys.argv[1] == 'scrub':
        d = bytearray(open(sys.argv[2], 'rb').read())
        d[512 + 441:1024] = bytes(1024 - 512 - 441)
        open(sys.argv[2], 'wb').write(d)
        return 0
    img = Image(sys.argv[2])
    if sys.argv[1] == 'info':
        for k, v in img.superblock().items():
            print('%-6s %s' % (k, v))
        return 0
    if sys.argv[1] == 'ls':
        path = sys.argv[3] if len(sys.argv) > 3 else '/'
        ino = img.lookup(path)
        if ino.kind() == IFDIR:
            for num, name in img.directory(ino):
                print(ls_line(img.inode(num), name))
        else:
            print(ls_line(ino, path))
        return 0
    if sys.argv[1] == 'cat' and len(sys.argv) > 3:
        sys.stdout.buffer.write(img.read(img.lookup(sys.argv[3])))
        return 0
    if sys.argv[1] == 'extract' and len(sys.argv) > 3:
        extract(img, sys.argv[3])
        return 0
    print(__doc__)
    return 2


if __name__ == '__main__':
    sys.exit(main())
