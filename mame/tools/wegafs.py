#!/usr/bin/env python3
"""wegafs.py - list and unpack WEGA (P8000, UNIX System III/V style) file systems from raw images.

The input is a RAW image: a floppy (convert CopyQM etc. first, e.g. "dskconv in.cqm out.raw -otype raw"
from libdsk) or a hard disk image (CHD: "chdman extractraw -i disk.chd -o disk.raw"), or any file that
holds a WEGA file system at a known offset.

Commands
  scan     IMAGE                   look for file systems (superblock at block 1, root directory plausible)
  info     IMAGE                   superblock and a short summary
  ls       IMAGE [PATH]            "ls -l" style listing, -R for a recursive listing
  cat      IMAGE PATH              write one file to standard output
  extract  IMAGE OUTDIR            unpack everything: directories, files, symbolic links, hard links,
                                   device nodes and FIFOs (see below), with mode, owner and time stamps
  check    IMAGE                   walk the whole tree and report inconsistencies

Common options
  --offset BYTES / --offset-blocks N   start of the file system inside the image (default 0); a floppy starts
                                       at 0, a hard disk partition at its first block (use "scan" to find it)
  --block-size N                       block size in bytes (default 512)

What "extract" keeps
  * the directory structure and all names
  * the permission bits including set-uid, set-gid and sticky
  * owner and group (numeric) - set with chown when run as root; otherwise they are only recorded in the manifest
  * access and modification time (utime); the change time cannot be set on Linux and is recorded in the manifest
  * hard links (a second name of the same inode is a hard link again)
  * sparse files (blocks that are not allocated stay holes)
  * device nodes: created with mknod when run as root, otherwise left out and listed in the manifest
  A manifest file "<OUTDIR>/.wegafs-manifest.tsv" lists every entry with inode, type, mode (octal), links, uid,
  gid, size, atime, mtime, ctime (seconds since 1970, UTC) and path, so nothing is lost when chown or mknod are
  not possible.  Use --no-manifest to suppress it.

Layout of the file system (as read from WEGA 3.1 disks; all values big-endian)
  block 0        boot block
  block 1        super block: s_isize (2) = number of i-node blocks, s_fsize (4) = size in blocks,
                 s_nfree (2), s_free[50] (4 each), s_ninode, s_inode[100], ...
  block 2..      i-nodes, 64 bytes each: mode(2) nlink(2) uid(2) gid(2) size(4) addr[13] (3 bytes each, 10 direct,
                 single, double, triple indirect) atime(4) mtime(4) ctime(4); i-node numbers start at 1,
                 the root directory is i-node 2
  directories    16 byte entries: i-node number (2), name (14, padded with NUL)
  indirect blocks: 128 block numbers of 4 bytes each (for 512 byte blocks)
"""

import argparse
import os
import stat
import struct
import sys
import time

ISIZE = 64            # bytes per i-node
DIRENT = 16
IFMT = 0o170000
IFDIR, IFREG, IFLNK, IFCHR, IFBLK, IFIFO = 0o040000, 0o100000, 0o120000, 0o020000, 0o060000, 0o010000


class FsError(Exception):
    pass


class Inode:
    __slots__ = ("num", "mode", "nlink", "uid", "gid", "size", "addr", "atime", "mtime", "ctime")

    @property
    def kind(self):
        return self.mode & IFMT

    @property
    def perm(self):
        return self.mode & 0o7777


class WegaFs:
    def __init__(self, path, offset=0, block_size=512):
        self.f = open(path, "rb")
        self.base = offset
        self.bs = block_size
        self.ppb = block_size // 4          # block numbers per indirect block
        self.warnings = []
        sb = self.block(1)
        self.s_isize, self.s_fsize = struct.unpack(">HI", sb[:6])
        if not self.plausible():
            raise FsError("no WEGA file system at this offset (use 'scan')")
        self.ninodes = self.s_isize * (self.bs // ISIZE)

    # -- low level ------------------------------------------------------------------------------------
    def block(self, n):
        self.f.seek(self.base + n * self.bs)
        d = self.f.read(self.bs)
        return d + b"\0" * (self.bs - len(d))

    def plausible(self):
        if not (0 < self.s_isize < 0x8000) or self.s_fsize <= self.s_isize + 2:
            return False
        try:
            root = self.inode(2)
        except FsError:
            return False
        if root.kind != IFDIR or root.size < 2 * DIRENT or root.size % DIRENT or root.nlink < 2:
            return False
        ents = self.read_dir(root, quiet=True)
        return bool(ents) and ents[0][1] == "." and ents[0][0] == 2 and len(ents) > 1 and ents[1][1] == ".." \
            and ents[1][0] == 2

    def warn(self, msg):
        self.warnings.append(msg)

    # -- i-nodes --------------------------------------------------------------------------------------
    def inode(self, num):
        if num < 1 or num > self.s_isize * (self.bs // ISIZE):
            raise FsError("i-node number %d out of range" % num)
        per = self.bs // ISIZE
        raw = self.block(2 + (num - 1) // per)[((num - 1) % per) * ISIZE:][:ISIZE]
        ino = Inode()
        ino.num = num
        ino.mode, ino.nlink, ino.uid, ino.gid, ino.size = struct.unpack(">HhHHI", raw[:12])
        ino.addr = [int.from_bytes(raw[12 + 3 * k:15 + 3 * k], "big") for k in range(13)]
        ino.atime, ino.mtime, ino.ctime = struct.unpack(">III", raw[52:64])
        return ino

    def _indirect(self, blk, level, out, limit):
        if len(out) >= limit:
            return
        if blk == 0:
            out.extend([0] * min(self.ppb ** level, limit - len(out)))
            return
        if blk >= self.s_fsize:
            self.warn("indirect block %d beyond the end of the file system" % blk)
            out.extend([0] * min(self.ppb ** level, limit - len(out)))
            return
        data = self.block(blk)
        for k in range(self.ppb):
            if len(out) >= limit:
                return
            p, = struct.unpack(">I", data[4 * k:4 * k + 4])
            if level == 1:
                out.append(p)
            else:
                self._indirect(p, level - 1, out, limit)

    def block_list(self, ino):
        """Data block numbers of the file (0 = hole), enough for ino.size bytes."""
        n = (ino.size + self.bs - 1) // self.bs
        out = list(ino.addr[:10])[:n]
        for level, a in ((1, ino.addr[10]), (2, ino.addr[11]), (3, ino.addr[12])):
            if len(out) >= n:
                break
            self._indirect(a, level, out, n)
        return out[:n]

    def read_file(self, ino):
        return b"".join(self.iter_file(ino))

    def iter_file(self, ino):
        remaining = ino.size
        for b in self.block_list(ino):
            chunk = min(self.bs, remaining)
            if b == 0:
                yield b"\0" * chunk
            elif b >= self.s_fsize:
                self.warn("i-node %d: block %d beyond the end of the file system" % (ino.num, b))
                yield b"\0" * chunk
            else:
                yield self.block(b)[:chunk]
            remaining -= chunk

    # -- directories ----------------------------------------------------------------------------------
    def read_dir(self, ino, quiet=False):
        data = self.read_file(ino)
        out = []
        for k in range(0, len(data) - DIRENT + 1, DIRENT):
            num, = struct.unpack(">H", data[k:k + 2])
            if num == 0:
                continue
            name = data[k + 2:k + DIRENT].split(b"\0")[0]
            out.append((num, name.decode("latin-1")))
        return out

    def walk(self, num=2, path="", seen=None):
        """Yield (path, inode, link target or None) depth first; directories before their content."""
        if seen is None:
            seen = set()
        ino = self.inode(num)
        yield path or "/", ino
        if ino.kind != IFDIR:
            return
        if num in seen:
            self.warn("directory loop at i-node %d" % num)
            return
        seen = seen | {num}
        for child, name in self.read_dir(ino):
            if name in (".", ".."):
                continue
            if "/" in name:
                self.warn("%s: name with '/' skipped" % path)
                continue
            try:
                sub = self.inode(child)
            except FsError as e:
                self.warn("%s/%s: %s" % (path, name, e))
                continue
            p = path + "/" + name
            if sub.kind == IFDIR:
                yield from self.walk(child, p, seen)
            else:
                yield p, sub

    def lookup(self, path):
        num = 2
        for part in [p for p in path.split("/") if p]:
            ino = self.inode(num)
            if ino.kind != IFDIR:
                raise FsError("%s is not a directory" % part)
            for c, n in self.read_dir(ino):
                if n == part:
                    num = c
                    break
            else:
                raise FsError("%s: no such file or directory" % path)
        return self.inode(num)


# -- helpers ---------------------------------------------------------------------------------------------
def mode_string(mode):
    t = {IFDIR: "d", IFREG: "-", IFLNK: "l", IFCHR: "c", IFBLK: "b", IFIFO: "p"}.get(mode & IFMT, "?")
    s = ""
    for i, (r, w, x) in enumerate((("r", "w", "x"),) * 3):
        shift = 6 - 3 * i
        bits = (mode >> shift) & 7
        s += (r if bits & 4 else "-") + (w if bits & 2 else "-") + (x if bits & 1 else "-")
    s = list(s)
    if mode & 0o4000:
        s[2] = "s" if s[2] == "x" else "S"
    if mode & 0o2000:
        s[5] = "s" if s[5] == "x" else "S"
    if mode & 0o1000:
        s[8] = "t" if s[8] == "x" else "T"
    return t + "".join(s)


def fmt_time(t):
    return time.strftime("%Y-%m-%d %H:%M", time.gmtime(t))


def long_line(ino, name, fs=None):
    size = ino.size
    if ino.kind in (IFCHR, IFBLK):
        dev = ino.addr[0]
        size = "%d,%d" % (dev >> 8, dev & 0xff)
    extra = ""
    if ino.kind == IFLNK and fs is not None:
        extra = " -> " + fs.read_file(ino).decode("latin-1")
    return "%s %3d %5d %5d %8s %s %s%s" % (mode_string(ino.mode), ino.nlink, ino.uid, ino.gid, size,
                                           fmt_time(ino.mtime), name, extra)


# -- commands --------------------------------------------------------------------------------------------
def open_fs(args):
    off = args.offset + args.offset_blocks * args.block_size
    return WegaFs(args.image, off, args.block_size)


def cmd_scan(args):
    size = os.path.getsize(args.image)
    step = args.block_size
    found = 0
    with open(args.image, "rb") as f:
        # cheap pre-check: the super block has s_isize/s_fsize that fit; the full check opens the root
        for off in range(0, size - 2 * step, step):
            f.seek(off + step)
            head = f.read(6)
            if len(head) < 6:
                break
            isz, fsz = struct.unpack(">HI", head)
            if not (0 < isz < 0x8000) or not (isz + 2 < fsz <= size // step):
                continue
            try:
                fs = WegaFs(args.image, off, step)
            except FsError:
                continue
            found += 1
            print("offset %d bytes = block %d: %d blocks (%d KiB), %d i-node blocks (%d i-nodes), root has %d entries"
                  % (off, off // step, fs.s_fsize, fs.s_fsize * step // 1024, fs.s_isize, fs.ninodes,
                     len(fs.read_dir(fs.inode(2)))))
    if not found:
        print("no file system found")
        return 1
    return 0


def cmd_info(args):
    fs = open_fs(args)
    root = fs.inode(2)
    files = dirs = other = 0
    for _, ino in fs.walk():
        if ino.kind == IFDIR:
            dirs += 1
        elif ino.kind == IFREG:
            files += 1
        else:
            other += 1
    print("block size      %d" % fs.bs)
    print("size            %d blocks (%d KiB)" % (fs.s_fsize, fs.s_fsize * fs.bs // 1024))
    print("i-node blocks   %d (%d i-nodes)" % (fs.s_isize, fs.ninodes))
    print("root            mode %s, mtime %s" % (mode_string(root.mode), fmt_time(root.mtime)))
    print("entries         %d directories, %d files, %d others" % (dirs, files, other))
    return 0


def cmd_ls(args):
    fs = open_fs(args)
    start = fs.lookup(args.path)
    if start.kind != IFDIR:
        print(long_line(start, args.path, fs))
        return 0
    if args.recursive:
        for p, ino in fs.walk(start.num, args.path.rstrip("/")):
            print(long_line(ino, p, fs))
    else:
        for num, name in fs.read_dir(start):
            if name in (".", "..") and not args.all:
                continue
            print(long_line(fs.inode(num), name, fs))
    return 0


def cmd_cat(args):
    fs = open_fs(args)
    ino = fs.lookup(args.path)
    if ino.kind != IFREG:
        raise FsError("%s is not a regular file" % args.path)
    for chunk in fs.iter_file(ino):
        sys.stdout.buffer.write(chunk)
    return 0


def cmd_extract(args):
    fs = open_fs(args)
    out = os.path.abspath(args.outdir)
    os.makedirs(out, exist_ok=True)
    root_user = hasattr(os, "geteuid") and os.geteuid() == 0
    can_chown = root_user or args.chown
    first = {}          # i-node number -> extracted path (for hard links)
    dirs = []           # (path, ino) for the time stamps, applied last
    manifest = []
    skipped = []
    chown_failed = False
    count = 0
    for p, ino in fs.walk():
        rel = p.lstrip("/")
        dest = os.path.join(out, rel) if rel else out
        kind = ino.kind
        manifest.append((ino, p))
        if kind == IFDIR:
            os.makedirs(dest, exist_ok=True)
            dirs.append((dest, ino))
            continue
        if ino.num in first and kind != IFDIR:
            try:
                os.link(first[ino.num], dest)
                continue
            except OSError as e:
                fs.warn("%s: hard link failed (%s), extracting a copy" % (p, e))
        first.setdefault(ino.num, dest)
        if kind == IFREG:
            with open(dest, "wb") as w:
                pos = 0
                for chunk in fs.iter_file(ino):
                    if chunk.count(0) == len(chunk):       # hole or zero block: keep the file sparse
                        pos += len(chunk)
                        w.seek(pos)
                    else:
                        w.write(chunk)
                        pos += len(chunk)
                w.truncate(ino.size)
        elif kind == IFLNK:
            target = fs.read_file(ino).decode("latin-1")
            if os.path.lexists(dest):
                os.unlink(dest)
            os.symlink(target, dest)
        elif kind in (IFCHR, IFBLK, IFIFO):
            try:
                if kind == IFIFO:
                    os.mkfifo(dest, ino.perm)
                else:
                    dev = ino.addr[0]
                    os.mknod(dest, ino.perm | (stat.S_IFCHR if kind == IFCHR else stat.S_IFBLK),
                             os.makedev(dev >> 8, dev & 0xff))
            except (PermissionError, OSError) as e:
                skipped.append(p)
                fs.warn("%s: %s node not created (%s), see the manifest" % (p, "device" if kind != IFIFO else "FIFO", e))
                continue
        else:
            fs.warn("%s: unknown file type %o, not extracted" % (p, kind))
            skipped.append(p)
            continue
        count += 1
        apply_attrs(dest, ino, can_chown)
        if can_chown is False:
            pass
    for dest, ino in reversed(dirs):                        # deepest first: the time stamps stay
        apply_attrs(dest, ino, can_chown)
    if not args.no_manifest:
        with open(os.path.join(out, ".wegafs-manifest.tsv"), "w") as m:
            m.write("inode\ttype\tmode\tlinks\tuid\tgid\tsize\tatime\tmtime\tctime\tpath\n")
            for ino, p in manifest:
                m.write("%d\t%s\t%04o\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\n" % (
                    ino.num, mode_string(ino.mode)[0], ino.perm, ino.nlink, ino.uid, ino.gid, ino.size,
                    ino.atime, ino.mtime, ino.ctime, p))
    print("%d entries extracted to %s (%d directories)" % (count + len(dirs), out, len(dirs)))
    if not can_chown:
        print("note: not running as root, owner and group are only in the manifest")
    if skipped:
        print("not created (see the manifest): " + ", ".join(skipped))
    for w in fs.warnings:
        print("warning: " + w, file=sys.stderr)
    return 0


def apply_attrs(path, ino, can_chown):
    link = ino.kind == IFLNK
    if can_chown:
        try:
            os.chown(path, ino.uid, ino.gid, follow_symlinks=False)
        except OSError:
            pass
    if not link:
        os.chmod(path, ino.perm)            # after chown: chown clears set-uid and set-gid
    try:
        os.utime(path, (ino.atime, ino.mtime), follow_symlinks=not link)
    except (OSError, NotImplementedError):
        pass


def cmd_check(args):
    fs = open_fs(args)
    seen = {}
    problems = 0
    total = 0
    for p, ino in fs.walk():
        total += 1
        seen[ino.num] = seen.get(ino.num, 0) + 1
        if ino.kind in (IFREG, IFDIR, IFLNK):
            nblocks = (ino.size + fs.bs - 1) // fs.bs
            bl = fs.block_list(ino)
            if len(bl) < nblocks:
                print("%s: size %d needs %d blocks, only %d found" % (p, ino.size, nblocks, len(bl)))
                problems += 1
            bad = [b for b in bl if b >= fs.s_fsize]
            if bad:
                print("%s: block number(s) beyond the file system: %s" % (p, bad[:4]))
                problems += 1
    for num, n in seen.items():
        ino = fs.inode(num)
        if ino.kind != IFDIR and ino.nlink != n:
            print("i-node %d: link count %d, but %d name(s) found" % (num, ino.nlink, n))
            problems += 1
    for w in fs.warnings:
        print("warning: " + w)
        problems += 1
    print("%d entries checked, %d problem(s)" % (total, problems))
    return 1 if problems else 0


def main():
    ap = argparse.ArgumentParser(description="List and unpack WEGA file systems from raw images.",
                                 formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    ap.add_argument("--offset", type=lambda s: int(s, 0), default=0, help="file system start in bytes")
    ap.add_argument("--offset-blocks", type=lambda s: int(s, 0), default=0, help="file system start in blocks")
    ap.add_argument("--block-size", type=int, default=512)
    sub = ap.add_subparsers(dest="cmd", required=True)
    for name, func in (("scan", cmd_scan), ("info", cmd_info), ("check", cmd_check)):
        p = sub.add_parser(name)
        p.add_argument("image")
        p.set_defaults(func=func)
    p = sub.add_parser("ls")
    p.add_argument("image")
    p.add_argument("path", nargs="?", default="/")
    p.add_argument("-R", "--recursive", action="store_true")
    p.add_argument("-a", "--all", action="store_true")
    p.set_defaults(func=cmd_ls)
    p = sub.add_parser("cat")
    p.add_argument("image")
    p.add_argument("path")
    p.set_defaults(func=cmd_cat)
    p = sub.add_parser("extract")
    p.add_argument("image")
    p.add_argument("outdir")
    p.add_argument("--no-manifest", action="store_true")
    p.add_argument("--chown", action="store_true", help="try chown even when not root")
    p.set_defaults(func=cmd_extract)
    args = ap.parse_args()
    try:
        return args.func(args)
    except FsError as e:
        print("wegafs: %s" % e, file=sys.stderr)
        return 2
    except BrokenPipeError:
        return 0


if __name__ == "__main__":
    sys.exit(main())
