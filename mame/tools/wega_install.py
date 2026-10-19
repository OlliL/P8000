#!/usr/bin/env python3
"""wega_install.py - install the WEGA 3.1 distribution floppies into a directory tree, like sa.install does.

Every WEGA installation floppy (root1..5, usr1..9) has a file CONTENTS in its root directory.  The stand-alone
program sa.install (WEGA/src/cmd/standalone/sa.install.c) reads it and copies the files from the floppy to the
hard disk.  This script reads the same CONTENTS and does the equivalent with install(1) on a Linux/macOS host:
directories are created with "install -d", files are copied with "install -m MODE -o UID -g GID", hard links
("l NAME") become "ln".

Usage
  wega_install.py [options] SOURCE...

  SOURCE   a directory that holds the files of one floppy including CONTENTS (e.g. made by "wegafs.py extract"),
           or a RAW floppy image (e.g. made with "dskconv in.cqm out.raw -otype raw").  Give the floppies in the
           order of the installation (root1 to root5 into the root prefix, usr1 to usr9 into the usr prefix).

  --prefix DIR   target directory (default: the current directory); the directories of the CONTENTS are created
                 below it ("/bin" becomes DIR/bin).  Use one prefix for the root file system and another for /usr.
  -n, --dry-run  only print the commands (a shell script that can be run with sudo)
  -p             keep the time stamps of the files on the floppy (install -p); without it install(1) uses the
                 current time, as sa.install does
  --owner / --no-owner   pass -o and -g to install (default: only when running as root)
  --devices      create the device nodes (b/c lines) with mknod; the WEGA major and minor numbers do not mean
                 anything on a Linux host, so they are skipped by default
  -v             print every command

CONTENTS format (from sa.install.c; lines end with LF, an empty line ends the file)
  /dir/sub                  the following lines apply to this directory ("/" is the top)
  d---775 UID GID NAME      directory
  ----755 UID GID FILE [NAME]  regular file: FILE is the name on the floppy, NAME the name on the disk (default FILE)
  n---644 UID GID FILE [NAME]  empty file (nothing is copied)
  b---640 UID GID MAJOR MINOR NAME   block device;   c...: character device
  l NAME                    another name (hard link) for the file or device that was created last
  Mode: type (- b c d n), then one letter each for set-uid (u), set-gid (g), sticky (t) or "-", then three octal
  digits.  UID, GID, MAJOR and MINOR are decimal.
"""

import argparse
import contextlib
import io
import os
import shlex
import subprocess
import sys
import tempfile
import types

HERE = os.path.dirname(os.path.abspath(__file__))


class ContentsError(Exception):
    pass


def split_lines(text):
    """CONTENTS lines the way sa.install sees them: CR and LF are separators, an empty line ends the file."""
    out = []
    for line in text.replace("\r\n", "\n").replace("\r", "\n").split("\n"):
        if line == "":
            break
        out.append(line)
    return out


def parse_mode(s, line):
    if len(s) != 7 or s[0] not in "-bcdn" or s[1] not in "-u" or s[2] not in "-g" or s[3] not in "-t":
        raise ContentsError("bad mode %r in %r" % (s, line))
    if any(c not in "01234567" for c in s[4:7]):
        raise ContentsError("bad octal digit in %r" % line)
    mode = int(s[4:7], 8)
    if s[1] == "u":
        mode |= 0o4000
    if s[2] == "g":
        mode |= 0o2000
    if s[3] == "t":
        mode |= 0o1000
    return s[0], mode


def number(s, line):
    if not s.isdigit():
        raise ContentsError("bad decimal number %r in %r" % (s, line))
    return int(s)


def parse(text):
    """Return a list of entries: ('dir', path) / ('d'|'-'|'n'|'b'|'c'|'l', ...)."""
    entries = []
    for line in split_lines(text):
        t = line.split()
        if not t:
            continue
        if line[0] == "/":
            comps = [c for c in line[1:].split("/") if c]
            entries.append(("cd", comps))
        elif line[0] == "l":
            if len(t) < 2:
                raise ContentsError("link without name: %r" % line)
            entries.append(("l", t[1]))
        elif line[0] in "-bcdn":
            kind, mode = parse_mode(t[0], line)
            if len(t) < 4:
                raise ContentsError("too few fields: %r" % line)
            uid, gid = number(t[1], line), number(t[2], line)
            if kind == "d":
                entries.append(("d", mode, uid, gid, t[3]))
            elif kind in "-n":
                if len(t) < 4:
                    raise ContentsError("no file name: %r" % line)
                src = t[3]
                dst = t[4] if len(t) > 4 else src
                entries.append(("n" if kind == "n" else "-", mode, uid, gid, src, dst))
            else:
                if len(t) < 6:
                    raise ContentsError("device line needs major, minor and name: %r" % line)
                entries.append((kind, mode, uid, gid, number(t[3], line), number(t[4], line), t[5]))
        else:
            raise ContentsError("syntax error in CONTENTS: %r" % line)
    return entries


def load_source(src, tmp):
    """Return the directory that holds the floppy's files (extracting a raw image if needed)."""
    if os.path.isdir(src):
        return src
    sys.path.insert(0, HERE)
    import wegafs
    out = os.path.join(tmp, "floppy%d" % len(os.listdir(tmp)))
    ns = types.SimpleNamespace(image=src, outdir=out, offset=0, offset_blocks=0, block_size=512,
                               no_manifest=True, chown=False)
    with contextlib.redirect_stdout(io.StringIO()):
        wegafs.cmd_extract(ns)
    return out


def find_file(directory, name):
    p = os.path.join(directory, name)
    if os.path.exists(p):
        return p
    try:
        for n in os.listdir(directory):          # the floppy names may differ in case
            if n.lower() == name.lower():
                return os.path.join(directory, n)
    except OSError:
        pass
    return None


class Runner:
    def __init__(self, args):
        self.args = args
        self.owner = args.owner if args.owner is not None else (hasattr(os, "geteuid") and os.geteuid() == 0)
        self.failed = 0
        self.skipped = []
        self.count = 0

    def run(self, cmd):
        a = self.args
        if a.dry_run or a.verbose:
            print(" ".join(shlex.quote(c) for c in cmd))
        if a.dry_run:
            return True
        r = subprocess.run(cmd)
        if r.returncode:
            self.failed += 1
            print("failed: " + " ".join(shlex.quote(c) for c in cmd), file=sys.stderr)
        return r.returncode == 0

    def ownopts(self, uid, gid):
        return ["-o", str(uid), "-g", str(gid)] if self.owner else []


def install_disk(entries, floppy_dir, prefix, r):
    cur = []                     # current directory as list of components
    last = None                  # path of the item created last (target of "l")
    a = r.args
    p_opt = ["-p"] if a.preserve else []
    for e in entries:
        kind = e[0]
        here = os.path.join(prefix, *cur)
        if kind == "cd":
            cur = e[1]
            here = os.path.join(prefix, *cur)
            if not os.path.isdir(here) and not a.dry_run:
                raise ContentsError("directory %s not found (as in sa.install: stop)" % ("/" + "/".join(cur)))
            print("/" + "/".join(cur))
        elif kind == "d":
            _, mode, uid, gid, name = e
            path = os.path.join(here, name)
            if os.path.isdir(path) and not a.dry_run:
                print("  directory %s already exists, not changed" % name)
                last = None
                continue
            r.run(["install", "-d", "-m", "%04o" % mode] + r.ownopts(uid, gid) + [path])
            last = path
            r.count += 1
        elif kind in "-n":
            _, mode, uid, gid, src, dst = e
            path = os.path.join(here, dst)
            if kind == "n":
                r.run(["install", "-m", "%04o" % mode] + r.ownopts(uid, gid) + [os.devnull, path])
            else:
                f = find_file(floppy_dir, src)
                if f is None:
                    print("  %s: not found on the floppy, skipped" % src, file=sys.stderr)
                    r.skipped.append(os.path.join("/" + "/".join(cur), dst))
                    r.failed += 1
                    last = None
                    continue
                r.run(["install"] + p_opt + ["-m", "%04o" % mode] + r.ownopts(uid, gid) + [f, path])
            last = path
            r.count += 1
        elif kind in "bc":
            _, mode, uid, gid, major, minor, name = e
            path = os.path.join(here, name)
            if not a.devices:
                r.skipped.append("%s %d,%d %s" % (kind, major, minor, os.path.join("/" + "/".join(cur), name)))
                last = False                     # skipped device: its links are skipped silently
                continue
            if r.run(["mknod", "-m", "%04o" % (mode & 0o777), path, kind, str(major), str(minor)]):
                if r.owner:
                    r.run(["chown", "%d:%d" % (uid, gid), path])
                last = path
                r.count += 1
            else:
                last = None
        elif kind == "l":
            path = os.path.join(here, e[1])
            if last is False:
                continue
            if last is None:
                print("  link %s: nothing to link to (previous item not installed), skipped" % e[1])
                continue
            r.run(["ln", "-f", last, path])
            r.count += 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", nargs="+")
    ap.add_argument("--prefix", default=".")
    ap.add_argument("-n", "--dry-run", action="store_true")
    ap.add_argument("-p", dest="preserve", action="store_true")
    ap.add_argument("--owner", dest="owner", action="store_true", default=None)
    ap.add_argument("--no-owner", dest="owner", action="store_false")
    ap.add_argument("--devices", action="store_true")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args()

    prefix = os.path.abspath(args.prefix)
    if not args.dry_run:
        os.makedirs(prefix, exist_ok=True)
    r = Runner(args)
    if not r.owner:
        print("note: not installing owner and group (not root); use sudo or --owner", file=sys.stderr)
    with tempfile.TemporaryDirectory() as tmp:
        for src in args.source:
            print("== %s" % src)
            d = load_source(src, tmp)
            cpath = find_file(d, "CONTENTS")
            if cpath is None:
                print("no CONTENTS on this floppy (a documentation or data disk?), skipped", file=sys.stderr)
                continue
            with open(cpath, "rb") as f:
                text = f.read().decode("latin-1")
            try:
                install_disk(parse(text), d, prefix, r)
            except ContentsError as e:
                print("error: %s" % e, file=sys.stderr)
                return 2
    print("%d item(s) installed into %s, %d problem(s)" % (r.count, prefix, r.failed))
    if r.skipped:
        print("not installed (%d): %s%s" % (len(r.skipped), ", ".join(r.skipped[:6]),
                                             " ..." if len(r.skipped) > 6 else ""))
    return 1 if r.failed else 0


if __name__ == "__main__":
    sys.exit(main())
