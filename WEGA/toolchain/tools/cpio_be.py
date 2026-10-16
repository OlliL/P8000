#!/usr/bin/env python3
"""List or extract an old binary cpio archive written big-endian (WEGA/Z8000).

Usage: cpio_be.py list ARCHIVE
       cpio_be.py extract ARCHIVE DESTDIR
Header: 13 16-bit words: magic 070707, dev, ino, mode, uid, gid, nlink, rdev,
mtime(2 words), namesize, filesize(2 words); name padded to even; data padded
to even.
"""
import os
import stat
import struct
import sys
import time


def entries(data):
    o = 0
    while o + 26 <= len(data):
        w = struct.unpack(">13H", data[o:o + 26])
        if w[0] != 0o70707:
            raise ValueError("bad magic at %#x: %o" % (o, w[0]))
        dev, ino, mode, uid, gid, nlink, rdev = w[1:8]
        mtime = (w[8] << 16) | w[9]
        namesize = w[10]
        fsize = (w[11] << 16) | w[12]
        o += 26
        name = data[o:o + namesize - 1].decode("latin-1")
        o += namesize + (namesize & 1)
        body = data[o:o + fsize]
        o += fsize + (fsize & 1)
        if name == "TRAILER!!!":
            return
        yield dict(dev=dev, ino=ino, mode=mode, uid=uid, gid=gid, nlink=nlink,
                   rdev=rdev, mtime=mtime, name=name, size=fsize, body=body, off=o)


def main():
    cmd, arc = sys.argv[1], sys.argv[2]
    data = open(arc, "rb").read()
    if cmd == "list":
        for e in entries(data):
            print("%06o %3d %3d/%-3d %8d %s ino=%d %s" % (
                e["mode"], e["nlink"], e["uid"], e["gid"], e["size"],
                time.strftime("%Y-%m-%d %H:%M", time.gmtime(e["mtime"])), e["ino"], e["name"]))
        return
    dest = sys.argv[3]
    seen = {}
    for e in entries(data):
        name = e["name"].lstrip("/")
        if name in ("", ".") or ".." in name.split("/"):
            continue
        p = os.path.join(dest, name)
        m = e["mode"]
        if stat.S_ISDIR(m):
            os.makedirs(p, exist_ok=True)
        elif stat.S_ISREG(m):
            os.makedirs(os.path.dirname(p), exist_ok=True)
            key = (e["dev"], e["ino"])
            if e["nlink"] > 1 and key in seen and e["size"] == 0:
                if os.path.exists(p):
                    os.remove(p)
                os.link(seen[key], p)
            else:
                with open(p, "wb") as f:
                    f.write(e["body"])
                seen[key] = p
        else:
            continue  # devices, fifos: skip
        try:
            os.chmod(p, stat.S_IMODE(m) | 0o600)
            os.utime(p, (e["mtime"], e["mtime"]))
        except OSError:
            pass


if __name__ == "__main__":
    main()
