#!/usr/bin/env python3
"""Disassemble a (nonsegmented, text+data) s.out object into PLZ/ASM (`as`)
source: s2as.py [-n modname] file.o

The module has one `global` procedure per global text symbol, local labels
L1.., data labels D<off> in an `internal` block, EPU instructions with their
Z8070 mnemonics (epu.py).  The result is meant to be assembled with the
original `as -f` and compared byte for byte with the object.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sdis  # noqa: E402
import epu  # noqa: E402

CC = {"eq": "z", "ne": "nz"}


def conv_line(line):
    m = re.match(r"\t\.word\t%([0-9A-F]{4}),%([0-9A-F]{4})\t// EPU", line)
    if m:
        t = epu.decode(int(m.group(1), 16), int(m.group(2), 16))
        if t is None:
            raise SystemExit("undecoded EPU %s" % line)
        return "\t" + t
    m = re.match(r"\t(\w+)\t(.*)$", line)
    if not m:
        return line
    mn, ops = m.groups()
    ops = ops.split("//")[0].rstrip()
    parts = ops.split(",") if ops else []
    if mn in ("jr", "jp", "ret", "call", "calr") and parts and parts[0] in CC and mn != "call":
        parts[0] = CC[parts[0]]
    if mn in ("cpir", "cpsir", "cpdr", "cpsdr", "cpirb", "cpsirb", "cpdrb", "cpsdrb") and parts and parts[-1] in CC:
        parts[-1] = CC[parts[-1]]
    if mn == "ret" and parts and parts[0] in CC:
        parts[0] = CC[parts[0]]
    return "\t" + mn + ("\t" + ",".join(parts) if parts else "")


def main(argv):
    name = None
    files = []
    i = 0
    while i < len(argv):
        if argv[i] == "-n":
            name = argv[i + 1]
            i += 2
        else:
            files.append(argv[i])
            i += 1
    fn = files[0]
    sdis.NOEXT = False
    txt = sdis.disasm(fn, False).split("\n")
    name = name or os.path.basename(fn).rsplit(".", 1)[0]
    ext = []
    text, data = [], []
    cur = None
    for l in txt:
        if l.startswith("\t.extern"):
            ext = [x.strip() for x in l.split("\t", 2)[2].split(",")]
        elif "data," in l and l.startswith("//"):
            cur = data
        elif "text," in l and l.startswith("//"):
            cur = text
        elif l.startswith("//") or l.startswith("\t.psec") or l.startswith("\t.code") \
                or l.startswith("\t.data") or l.startswith("\t.seg") or not l.strip():
            continue
        elif l.startswith("\t.comm"):
            raise SystemExit("common symbols not supported")
        elif cur is not None:
            cur.append(l)
    # local label numbering in order of appearance
    lab = {}
    for l in text:
        if re.match(r"L\d+t_[0-9a-f]+:$", l):
            lab[l[:-1]] = "L%d" % (len(lab) + 1)
    dlab = {}
    for l in data:
        mm = re.match(r"(L\d+d_)([0-9a-f]+):$", l)
        if mm:
            dlab[l[:-1]] = "D%s" % mm.group(2)

    def rn(s):
        for k, v in sorted(lab.items(), key=lambda x: -len(x[0])):
            s = s.replace(k, v)
        for k, v in sorted(dlab.items(), key=lambda x: -len(x[0])):
            s = s.replace(k, v)
        return s

    out = ["%s module" % name, ""]
    if ext:
        out.append("  external")
        for e in ext:
            out.append("    %s\tprocedure" % e)
        out.append("")
    if data:
        out.append("  internal")
        for l in data:
            if l.endswith(":") and not l.endswith("::"):
                out.append("    %s" % rn(l))
            else:
                mm = re.match(r"\t\.word\t(.*)$", l)
                v = rn(mm.group(1).split("//")[0].strip())
                if v.startswith("%"):
                    v = "%" + v[1:].lower()
                out.append("\tWORD := %s" % v)
        out.append("")
    out.append("  global")
    procs = []
    for l in text:
        if l.endswith("::"):
            procs.append([l[:-2], []])
        elif procs:
            procs[-1][1].append(l)
    for pname, body in procs:
        out.append("    %s procedure" % pname)
        out.append("      entry")
        for l in body:
            if re.match(r"L\d+t_[0-9a-f]+:$", l):
                out.append(rn(l))
            else:
                out.append(rn(conv_line(l)))
        out.append("    end %s" % pname)
        out.append("")
    out.append("end %s" % name)
    print("\n".join(out))


if __name__ == "__main__":
    main(sys.argv[1:])
