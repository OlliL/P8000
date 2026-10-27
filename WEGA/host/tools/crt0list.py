#!/usr/bin/env python3
"""Start-up files (crt0) of the original programs.

Usage: crt0list.py TREE            tab separated list: name, text, revision, stamp, path
       crt0list.py -v TREE         also list the programs that have another start-up code

The crt0 of the programs of the system is one piece of code (jr over a text, then the usual
start-up) that differs in three places only: the 21 characters of the text behind the jump, the
revision and the date/time of the SCCS stamp in the data (`@[$]crt0.s <tab><tab>Rev : x <tab> date`).
The stamp is the build time of the program, so it differs from program to program. Some programs have the
code but no stamp (revision and stamp empty in the list). The list is
read by src/lib/libc/crt/mkcrt0.sh together with crt0.s.in.
"""
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from sout import SOut  # noqa: E402
import libcver  # noqa: E402

STAMP = re.compile(rb"^\x00\x00@\[\$\]crt0\.s\t\tRev : (\S+)\t(\d\d/\d\d/\d\d \d\d:\d\d:\d\d)\x00")
STAMP_S = re.compile(rb"^\x00\x00@\[\$\]scrt0\.s\t\tRev : (\S+)\t(\d\d/\d\d/\d\d \d\d:\d\d:\d\d)\x00")
# start-up code of the segmented programs (src/lib/libc/crt/scrt0.s.in): a jump and the text as in crt0, the code
# starts at 0x18 with dec r15,#4; the byte at 0x3c is the segment of _environ (0x82: segment 2, 0x80: segment 0)
SCRT0_HEAD = b"\xab\xf3\x61\xf7"


def scan(tree, verbose=False):
    ref = None
    rows, other = [], []
    global why
    why = {}
    for dp, dn, fn in sorted(os.walk(tree)):
        dn.sort()
        for f in sorted(fn):
            p = os.path.join(dp, f)
            try:
                d = open(p, "rb").read()
                s = SOut(d)
            except (ValueError, OSError, struct.error):
                continue
            if s.magic not in (0xE607, 0xE611, 0xE707, 0xE711):
                continue
            g = s.segs[0]
            if g["code"] < 0x40:
                continue
            img = s.image
            if img[0x18:0x1c] == SCRT0_HEAD and img[0] == 0xE8 and img[1] == 0x0B:
                data = img[g["code"]:g["code"] + 64]
                m = STAMP_S.match(data)
                if not m:
                    other.append(p)
                    why[p] = "scrt0 start without stamp"
                    continue
                envseg = {0x82: "2", 0x80: "0"}.get(img[0x3c], "?")
                text = img[2:0x17].decode("latin-1")
                rows.append((f, text, m.group(1).decode(), m.group(2).decode(), os.path.relpath(p, tree),
                             "scrt0", envseg))
                continue
            # code without the text: the jr at 0, the text at 2..0x17, code from 0x18
            if img[0] != 0xE8 or img[1] != 0x0B:
                other.append(p)
                why[p] = "no jump over a text at the start"
                continue
            body = [libcver.norm(t) for pc, t in libcver.decode(img[0x18:0x40], 0x28)]
            if ref is None:
                ref = body
            if body[:12] != ref[:12]:
                other.append(p)
                why[p] = "other code"
                continue
            text = img[2:0x17]
            data = img[g["code"]:g["code"] + 64]
            m = STAMP.match(data)
            if not m and data[:2] == b"\x00\x00" and b"@[$]crt0" not in d:
                rows.append((f, text.decode("latin-1"), "", "", os.path.relpath(p, tree)))   # start-up code without stamp
                continue
            if not m:
                other.append(p)
                why[p] = "stamp not at the start of the data" if b"crt0.s" in d else "no crt0 stamp"
                continue
            rows.append((f, text.decode("latin-1"), m.group(1).decode(), m.group(2).decode(), os.path.relpath(p, tree)))
    return rows, other


def main():
    a = sys.argv[1:]
    verbose = False
    if a and a[0] == "-v":
        verbose = True
        a.pop(0)
    if not a:
        print(__doc__)
        return 1
    rows, other = scan(a[0], verbose)
    seen = {}
    for r in rows:
        print("\t".join(r))
        seen.setdefault(r[0], []).append(r[4])
    for k, v in seen.items():
        if len(v) > 1:
            print("# name used twice:", k, v, file=sys.stderr)
    if verbose:
        for p in other:
            print("# other start-up:", p, "-", why.get(p), file=sys.stderr)
    print("# %d programs, %d with another start-up code" % (len(rows), len(other)), file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
