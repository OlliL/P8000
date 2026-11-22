#!/usr/bin/env python3
"""Write the code generator table (table3.c) of /lib/codgen or /lib/scodgen as C source.

Usage: codgentab.py /lib/codgen > table3.nzcc.c   (/lib/scodgen > table3.szcc.c; table3.c includes one of them)
       codgentab.py --numbers /lib/codgen          (the fields as numbers, not as the names of mfile3 and mip/manifest)
       codgentab.py --ops /lib/codgen > ops.c      (the operator table with the dope flags)

The table of the PCC second pass is an array of 10 word entries
  op visit lshape ltype rshape rtype needs nx rewrite cstring
that follows the version string "table3.nzcc.c" (codgen) or "table3.szcc.c" (scodgen) in the data;
cstring points to the template text. Entries are printed with their numbers so that the
comments /* nnn */ of the templates can be found again.
"""
import re
import struct
import sys

sys.path.insert(0, __import__("os").path.dirname(__file__))
from sout import SOut

NAMES = ("op", "visit", "lshape", "ltype", "rshape", "rtype", "needs", "nx", "rewrite")

HERE = __import__("os").path.dirname(__import__("os").path.abspath(__file__))
MANIFEST = HERE + "/../../src/cmd/cc/mip/manifest"

# the names of the fields (mfile3; the pseudo operators in the order of ocnames of match3.c, the WEGA strings)
OPCLASS = ("OPSIMP", "OPCOMM", "OPMUL", "OPDIV", "OPUNARY", "OPLEAF", "OPANY", "OPLOG", "OPLONG", "OPSHFT", "OPBIT",
           "OPFLOAT", "OPLTYPE")
COOKIES = ((0x4000, "FORREW"), (0x2000, "FORARG"), (0x1000, "INTEMP"), (0x20, "FORCC"), (0x10, "INTBREG"),
           (0x8, "INBREG"), (0x4, "INTAREG"), (0x2, "INAREG"), (0x1, "FOREFF"))
SHAPES = ((0x4000, "SWADD"), (0x2000, "FORARG"), (0x1000, "INTEMP"), (0x800, "STARREG"), (0x400, "STARNM"),
          (0x200, "SOREG"), (0x100, "SFLD"), (0x80, "SCON"), (0x40, "SNAME"), (0x20, "SCC"), (0x10, "STBREG"),
          (0x8, "SBREG"), (0x4, "STAREG"), (0x2, "SAREG"), (0x1, "SANY"))
SPECIALS = {0x8000: "SZERO", 0x8001: "SKCON", 0x8002: "S16CON", 0x8003: "SBCON", 0x8004: "SWCON", 0x8005: "SWNCON",
            0x8006: "SPOW2", 0x8007: "SNPOW2", 0x8008: "SADCON"}
TYPES = ((0x8000, "TFIT"), (0x4000, "TSAME"), (0x2000, "TSTRUCT"), (0x1000, "TANY"), (0x800, "TPTRTO"),
         (0x400, "TULONG"), (0x200, "TUNSIGNED"), (0x100, "TUSHORT"), (0x80, "TUCHAR"), (0x40, "TPOINT"),
         (0x20, "TDOUBLE"), (0x10, "TFLOAT"), (0x8, "TLONG"), (0x4, "TINT"), (0x2, "TSHORT"), (0x1, "TCHAR"))
REWRITES = ((0x1000, "RNOP"), (0x800, "RESCC"), (0x400, "RESC"), (0x10, "RESC3"), (0x8, "RESC2"), (0x4, "RESC1"),
            (0x2, "RRIGHT"), (0x1, "RLEFT"))
BINOPS = ("PLUS", "MINUS", "MUL", "AND", "OR", "ER", "DIV", "MOD", "LS", "RS")
UNOPS = {"MINUS": "UNARY MINUS", "MUL": "UNARY MUL", "AND": "UNARY AND", "CALL": "UNARY CALL"}
FREE = 100


def opnames():
    """value -> name of the operators of mip/manifest (ERROR .. LAB), with ASG and UNARY"""
    ops = {}
    on = False
    for l in open(MANIFEST):
        m = re.match(r"#\s*define\s+(\w+)\s+(\d+)\b", l)
        if not m:
            continue
        if m.group(1) == "ERROR":
            on = True
        if on:
            ops.setdefault(int(m.group(2)), m.group(1))
        if m.group(1) == "LAB":
            break
    val = {v: k for k, v in ops.items()}
    for b in BINOPS:
        ops.setdefault(val[b] + 1, "ASG " + b)
    for b, u in UNOPS.items():
        ops.setdefault(val[b] + 2, u)
    return ops


def flags(x, table):
    if x == 0:
        return "0"
    out = []
    for bit, name in table:
        if x & bit:
            out.append(name)
            x &= ~bit
    if x:
        out.append("0x%x" % x)
    return "|".join(reversed(out))


def shape(x):
    if x & 0x8000:
        return SPECIALS.get(x, "SPECIAL|%d" % (x & 0x7fff))
    return flags(x, SHAPES)


def needs(x):
    if x == 0x1000:
        return "REWRITE"
    out = []
    for cnt, mask, name in ((0x1, 0x3, "NAREG"), (0x10, 0x30, "NBREG"), (0x100, 0x300, "NTEMP"), (0x400, 0xc00, "NPTEMP")):
        n = (x & mask) // cnt
        if n:
            out.append(name if n == 1 else "%d*%s" % (n, name))
    for bit, name in ((0x4, "NASL"), (0x8, "NASR"), (0x40, "NBSL"), (0x80, "NBSR")):
        if x & bit:
            out.append(name)
    rest = x & ~0xfff
    if rest:
        out.append("0x%x" % rest)
    return "|".join(out) if out else "0"


def named(w, ops):
    """the nine fields of an entry by their names"""
    if w[:7] == (FREE,) * 7:
        return ["FREE"] * 7 + ["0x%x" % w[7], "FREE"]
    op = w[0]
    opn = ("ASG " if op & 1 else "") + OPCLASS[(op - 0x1000) // 2] if op >= 0x1000 else ops[op]
    if w[6] == 0x1000:	# a rewrite rule: the answer is an operator (BITYPE for a binary one)
        rw = "BITYPE" if w[8] == 8 else ops[w[8]]
    else:
        rw = flags(w[8], REWRITES) if w[8] else "RNULL"
    return [opn, flags(w[1], COOKIES), shape(w[2]), flags(w[3], TYPES), shape(w[4]), flags(w[5], TYPES), needs(w[6]),
            "0x%x" % w[7], rw]


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


def main(path, numbers=False):
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
    prog = path.split("/")[-1]
    print("/* the table of %s (table3.c includes it): %d entries, found at data offset 0x%x */" % (prog, len(ents), start))
    stamps = re.findall(rb"@\[\$\]table3[^\0]*", d)
    print("static char tabsccs[] = \"%s\";" % stamps[1].decode().replace("\t", "\\t"))
    print()
    print("struct optab table[] = {")
    ops = opnames()
    for n, w in enumerate(ents):
        e = d.index(b"\0", w[9])
        if numbers:
            print("\t{ %s, %s },\t/* %d */" % (", ".join("0x%x" % x for x in w[:9]), cstr(d[w[9]:e]), n))
        else:
            f = named(w, ops)
            print("\t{ %s,\t%s,\t\t/* %d */\n\t\t%s,\t%s,\n\t\t%s,\t%s,\n\t\t%s,\t%s,\t%s,\n\t\t%s },"
                  % (f[0], f[1], n, f[2], f[3], f[4], f[5], f[6], f[7], f[8], cstr(d[w[9]:e])))
    print("};")


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
    print("struct indope indope[] = {")
    for op, name, flags in ents:
        print("\t{ %d, %s, 0x%x }," % (op, cstr(name), flags))
    print("\t{ -1, \"\", 0 }\n};")


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
    elif sys.argv[1] == "--numbers":
        main(sys.argv[2], True)
    else:
        main(sys.argv[1])
