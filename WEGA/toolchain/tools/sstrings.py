#!/usr/bin/env python3
"""Print NUL-terminated printable strings from the data sections of an
s.out file, with their offset inside the data image (per segment).

Usage: sstrings.py [-n MIN] file ...
"""
import re
import sys

sys.path.insert(0, __import__("os").path.dirname(__file__))
from sout import SOut  # noqa: E402


def main(argv):
    mn = 3
    if argv and argv[0] == "-n":
        mn, argv = int(argv[1]), argv[2:]
    pat = re.compile(rb"[\x20-\x7e\t\n\r\x08\x0c]{%d,}\x00" % mn)
    for fn in argv:
        s = SOut(open(fn, "rb").read())
        if len(argv) > 1:
            print("== " + fn)
        for si, kind, off, size in s.section_ranges():
            if kind != "data":
                continue
            blob = s.image[off:off + size]
            for m in pat.finditer(blob):
                print("seg%d+%05x %r" % (s.segs[si]["segno"], m.start(),
                                         m.group()[:-1].decode("latin-1")))


if __name__ == "__main__":
    try:
        main(sys.argv[1:])
    except BrokenPipeError:
        pass
