#!/usr/bin/env python3
"""wegafs.py - the WEGA file system (System III) in an image: a floppy, a partition of a hard disk.

  wegafs.py info IMAGE               the super block: size, i-node blocks, free blocks and i-nodes, name, pack
  wegafs.py ls IMAGE [PATH]          a directory (or one file) as ls -l: mode, links, uid, gid, size, time, name
  wegafs.py cat IMAGE PATH           the data of a file on stdout
  wegafs.py extract IMAGE DIR        the whole tree into DIR (made): directories, files, links (a second name of an
                                     i-node is a hard link again), modes, times; the owners and what the host cannot
                                     make (device files) are listed in DIR.owners: uid gid mode path

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
- A directory: entries of 16 bytes, the i-node number (2, 0: free) and the name (14, DIRSIZ, filled with NUL)."""
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
