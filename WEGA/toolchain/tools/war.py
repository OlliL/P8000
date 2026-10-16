#!/usr/bin/env python3
"""Reader for WEGA ar archives (ar(5)): magic 0177545, 26 byte headers.

Usage: war.py list LIB          table of contents with header fields
       war.py extract LIB DIR   extract members (mtime/mode kept)
As a module: members(data) yields dicts with name, date, uid, gid, mode,
size, off (offset of member data), body.
"""
import os
import struct
import sys
import time

ARMAG = 0o177545


def members(data):
    if struct.unpack(">H", data[:2])[0] != ARMAG:
        raise ValueError("not a WEGA ar archive")
    o = 2
    while o + 26 <= len(data):
        name, date, uid, gid, mode, size = struct.unpack(">14sIBBHI", data[o:o + 26])
        name = name.split(b"\0")[0].decode("latin-1")
        body = data[o + 26:o + 26 + size]
        yield dict(name=name, date=date, uid=uid, gid=gid, mode=mode, size=size,
                   hdr=o, off=o + 26, body=body)
        o += 26 + size + (size & 1)
    if o != len(data):
        raise ValueError("archive ends at %d, file has %d bytes" % (o, len(data)))


def main():
    cmd, lib = sys.argv[1], sys.argv[2]
    data = open(lib, "rb").read()
    if cmd == "list":
        for m in members(data):
            print("%-14s %8d %06o %3d/%-3d %s hdr@%d" % (
                m["name"], m["size"], m["mode"], m["uid"], m["gid"],
                time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(m["date"])), m["hdr"]))
    elif cmd == "extract":
        dest = sys.argv[3]
        os.makedirs(dest, exist_ok=True)
        for m in members(data):
            n = m["name"].replace("/", "_") or "_noname"
            p = os.path.join(dest, n)
            with open(p, "wb") as f:
                f.write(m["body"])
            os.utime(p, (m["date"], m["date"]))


if __name__ == "__main__":
    try:
        main()
    except BrokenPipeError:
        pass
