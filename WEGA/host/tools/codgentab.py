#!/usr/bin/env python3
"""Write the code generator table (table3.c) of /lib/codgen or /lib/scodgen as C source.

Usage: codgentab.py /lib/codgen > table3.c        (the file of the original system)
       codgentab.py --ops /lib/codgen > ops.c      (the operator table with the dope flags)

The table of the PCC second pass is an array of 10 word entries
  op visit lshape ltype rshape rtype needs nx rewrite cstring
that follows the version string "table3.nzcc.c" (codgen) or "table3.szcc.c" (scodgen) in the data;
cstring points to the template text. Entries are printed with their numbers so that the
comments /* nnn */ of the templates can be found again.
"""
import struct
import sys

sys.path.insert(0, __import__("os").path.dirname(__file__))
from sout import SOut

NAMES = ("op", "visit", "lshape", "ltype", "rshape", "rtype", "needs", "nx", "rewrite")


def cstr(b):
    out = []
    for c in b:
        ch = chr(c)
        if ch == "\n":
            out.append("\\n")
        elif ch == "\t":
            out.append("\\t")
        elif ch == '"':
            out.append('\\"')
        elif ch == "\\":
            out.append("\\\\")
        elif 32 <= c < 127:
            out.append(ch)
        else:
            out.append("\\%03o" % c)
    return '"' + "".join(out) + '"'


def main(path):
    s = SOut(open(path, "rb").read())
    d = s.image[s.segs[0]["code"]:]
    tag = b"table3.n" if b"table3.nzcc.c" in d else b"table3.s"
    i = d.index(tag + b"zcc.c")
    i = d.index(b"\0", i) + 1
    if i & 1:
        i += 1
    # the first entry: ten words, the last one points at a template ("/* 000 */")
    start = i
    ents = []
    while True:
        w = struct.unpack(">10H", d[i:i + 20])
        p = w[9]
        if not (p < len(d) and (d[p:p + 2] == b"/*" or d[p] == 0)):
            break
        ents.append(w)
        i += 20
        if w[:7] == (0x64,) * 7:	# the last entry: every field 0x64, template "/*NULL*/"
            break
    print("/* %s: %d entries, found at data offset 0x%x */" % (path.split("/")[-1], len(ents), start))
    print("#include \"cg.h\"\n")
    print("struct optab table[] = {")
    for n, w in enumerate(ents):
        e = d.index(b"\0", w[9])
        print("\t{ %s, %s },\t/* %d */" % (", ".join("0x%x" % x for x in w[:9]), cstr(d[w[9]:e]), n))
    print("};")
    print("int ntable = %d;" % len(ents))


def ops(path):
    """The operator table (12 byte entries: op, name[8], flags) as C source: dope flags and names of the operators."""
    s = SOut(open(path, "rb").read())
    d = s.image[s.segs[0]["code"]:]
    i = d.index(b"\0\0\0\2NAME\0") + 2
    ents = []
    while True:
        op, = struct.unpack(">H", d[i:i + 2])
        name = d[i + 2:i + 10].split(b"\0")[0]
        flags, = struct.unpack(">H", d[i + 10:i + 12])
        if op == 0 or not name:
            break
        ents.append((op, name, flags))
        i += 12
    print("/* %s: %d operators (op, name, dope flags) */" % (path.split("/")[-1], len(ents)))
    print("#include \"cg.h\"\n")
    print("struct indope indope[] = {")
    for op, name, flags in ents:
        print("\t{ %d, 0x%x, %s }," % (op, flags, cstr(name)))
    print("\t{ 0, 0, 0 }\n};")


def hop(path):
    """The hoptab tables (op, mnemonic, type word; 6 bytes each, ended by a negative op) as C source."""
    s = SOut(open(path, "rb").read())
    d = s.image[s.segs[0]["code"]:]
    starts = []
    pos = 0
    while True:
        i = d.find(b"fadd\0", pos)
        if i < 0:
            break
        pos = i + 1
        ent = struct.pack(">HH", 7, i)
        j = d.find(ent)
        if j >= 0 and struct.unpack(">H", d[j + 4:j + 6])[0] == 0x30:
            starts.append(j)
    if not starts:
        raise SystemExit("hoptab not found")
    f = starts[0]
    tabs = []
    a = f
    for _ in range(2):
        ents = []
        while True:
            op, sp, tw = struct.unpack(">hHH", d[a:a + 6])
            a += 6
            if op < 0:
                break
            ents.append((op, d[sp:d.index(b"\0", sp)], tw))
        tabs.append(ents)
    print("/* %s: hoptab, floating and integer */" % path.split("/")[-1])
    print("struct hoptab { int op; char *str; int tword; };\n")
    for name, ents in zip(("fhoptab", "ihoptab"), tabs):
        print("struct hoptab %s[] = {" % name)
        for op, st, tw in ents:
            print("\t{ %d, %s, 0x%x }," % (op, cstr(st), tw))
        print("\t{ -1, 0, 0 }\n};\n")


if __name__ == "__main__":
    if sys.argv[1] == "--hop":
        hop(sys.argv[2])
    elif sys.argv[1] == "--ops":
        ops(sys.argv[2])
    else:
        main(sys.argv[1])
