#!/usr/bin/env python3
"""Cut the libc part of a linked WEGA program into assembler modules, one per libc module.

Usage: unlink.py SPEC PROGRAM OUTDIR OWNOBJ.o ...

  SPEC      description of the libc part (see below)
  PROGRAM   the original program (nonsegmented, I&D combined)
  OUTDIR    the old libc directory (src/lib/oldlibc): a module NAME becomes OUTDIR/NAME/0_<n>.s,
            n counts the different versions of the module (the same text gets the same n)
  OWNOBJ.o  the own objects (crt0 and the program), in link order; their relocations give the names
            and addresses of the symbols that the own code uses

Also keeps OUTDIR/VERSIONS, the table of which program contains which version of a module.
Prints the link order: "old NAME 0_n" for a module that was cut out, "shipped NAME" for a module
that is the member NAME.o of the shipped libc.a.

SPEC, one line per libc module in link order, fields separated by blanks, # starts a comment:

    NAME KIND START END DATA [SYMBOL=ADDRESS ...]

  KIND      old      the module is cut out and written as assembler
            shipped  the module is the member NAME.o of the shipped libc.a (WEGA=dir of the original
                     tree); it is only used to find symbol names
  START,END text range (hex), - for a module without text (data only)
  DATA      size of the data of the module in bytes
  dstart=HEX  address of the data of the module (optional). Without it the data follows the data of the
            modules before it: combined programs (E707) end at the data of the program, separate ones
            (E711, E611) start at address 0 (the data space of the program)
  SYMBOL=ADDRESS   a global symbol of the module (address in the program, hex); comm:NAME=ADDRESS:SIZE
                   is a common (bss) that the module defines

Other lines: "noown" (do not take symbol addresses from the relocations of the own objects, for programs
whose own code differs from the original) and "sym NAME=ADDRESS ..." (global names of the own part).

A module is cut out as it is: the assembler cannot relocate relative branches (jr, djnz, calr) or
jump tables to another object, so everything they connect has to be in one module. Calls and
data/bss references to other modules become symbols. Local labels count from the start of the
module, so the same module in another program gives the same text. The result is linked in the
order of the spec behind the own objects and gives the original program byte for byte.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import libcver  # noqa: E402
import war  # noqa: E402
from sout import SOut  # noqa: E402

HEX = re.compile(r"%([0-9A-F]{4,8})")
FLAGS = {}


def own_symbols(prog, objs):
    """name -> address of the symbols that the relocations of the own objects name"""
    syms = {}
    toff = 0
    for path in objs:
        o = SOut(open(path, "rb").read())
        g = o.segs[0]
        for a in range(0, len(o.reloc) - 1, 2):
            rw = (o.reloc[a] << 8) | o.reloc[a + 1]
            if not rw or not rw & 8 or a >= g["code"]:
                continue
            y = o.symidx.get(rw >> 4)
            if not y:
                continue
            add = (o.image[a] << 8) | o.image[a + 1]
            val = (prog.image[toff + a] << 8) | prog.image[toff + a + 1]
            syms.setdefault(y["name"], []).append((val - add) & 0xFFFF)
        toff += g["code"]
    return {n: max(set(v), key=v.count) for n, v in syms.items()}


def own_commons(objs):
    out = {}
    for path in objs:
        o = SOut(open(path, "rb").read())
        for y in o.symbols:
            if (y["type"] & 0x1F) == 0 and y["value"]:
                out[y["name"]] = y["value"]
    return out


def read_spec(path):
    mods = []
    for line in open(path):
        line = line.split("#")[0].split()
        if not line:
            continue
        if line[0] == "noown":
            FLAGS["noown"] = True
            continue
        if line[0] == "sym":
            for f in line[1:]:
                n, ad = f.split("=")
                FLAGS.setdefault("sym", {})[n] = int(ad, 16)
            continue
        name, kind, a, b, data = line[:5]
        m = dict(name=name, kind=kind, start=None if a == "-" else int(a, 16),
                 end=None if b == "-" else int(b, 16), data=int(data), syms={}, comms={})
        dstart = None
        for f in line[5:]:
            if f.startswith("dstart="):
                dstart = int(f[7:], 16)
                continue
            if f.startswith("comm:"):
                n, rest = f[5:].split("=")
                ad, sz = rest.split(":")
                m["comms"][n] = (int(ad, 16), int(sz))
            else:
                n, ad = f.split("=")
                m["syms"][n] = int(ad, 16)
        if dstart is not None:
            m["dstart"] = dstart
        mods.append(m)
    return mods


def main():
    args = sys.argv[1:]
    if len(args) < 4:
        print(__doc__)
        return 1
    spec, path, outdir, objs = args[0], args[1], args[2], args[3:]
    mods = read_spec(spec)
    prog = SOut(open(path, "rb").read())
    g = prog.segs[0]
    tsize, dsize, bsize = g["code"], g["data"], g["bss"]
    # separate I and D space (E611, E711, E511): data addresses start at 0, bss follows the data;
    # combined (E607, E707, E507): data follows the code, bss follows the data (the addresses are in one space)
    separate = prog.magic in (0xE611, 0xE711, 0xE511)
    dbase = 0 if separate else tsize
    bbase = dsize if separate else tsize + dsize
    bend = bbase + bsize
    img = prog.image
    own = {} if FLAGS.get("noown") else own_symbols(prog, objs)
    own.update(FLAGS.get("sym", {}))
    ocomm = own_commons(objs)
    ownend = min(m["start"] for m in mods if m["start"] is not None)
    lib = {}
    root = os.environ.get("WEGA")
    if root and any(m["kind"] == "shipped" for m in mods):
        for mm in war.members(open(os.path.join(root, "lib/libc.a"), "rb").read()):
            if mm["name"].endswith(".o"):
                lib[mm["name"][:-2]] = SOut(mm["body"])
    # data of the modules: an explicit dstart in the spec, otherwise the modules follow each other
    # (combined: ending at bbase; separate: starting at dbase, in link order)
    data_total = sum(m["data"] for m in mods if "dstart" not in m)
    dcur = dbase if separate else bbase - data_total
    for m in mods:
        if "dstart" not in m:
            m["dstart"] = dcur
            dcur += m["data"]
    names = dict((a, n) for n, a in own.items())
    for m in mods:
        for n, a in m["syms"].items():
            names[a] = n
        for n, (a, sz) in m["comms"].items():
            names[a] = n
        if m["kind"] == "shipped" and m["name"] in lib:
            for y in lib[m["name"]].symbols:
                t = y["type"] & 0x1F
                if t == 2 and m["start"] is not None:
                    names.setdefault(m["start"] + y["value"], y["name"])
                elif t == 3:
                    names.setdefault(m["dstart"] + y["value"], y["name"])
    ins = [(pc, t) for pc, t in libcver.decode(img, tsize) if pc >= ownend]
    starts = {pc for pc, t in ins}

    def dimg(a):
        """index in the image of a data address (separate I and D: the data follows the code in the image)"""
        return a - dbase + tsize

    def module_of(addr):
        for m in mods:
            if m["start"] is not None and m["start"] <= addr < m["end"]:
                return m
        return None

    def dmodule_of(addr):
        for m in mods:
            if m["data"] and m["dstart"] <= addr < m["dstart"] + m["data"]:
                return m
        return None

    tables = {}       # data address -> text address, for the jump tables
    for m in mods:
        if m["kind"] != "old" or not m["data"]:
            continue
        k, hi = m["dstart"], m["dstart"] + m["data"]
        while k + 4 <= hi:
            j = k
            while j + 2 <= hi:
                w = (img[dimg(j)] << 8) | img[dimg(j) + 1]
                if w in starts and module_of(w) is m:
                    j += 2
                else:
                    break
            if j - k >= 4:
                for q in range(k, j, 2):
                    tables[q] = (img[dimg(q)] << 8) | img[dimg(q) + 1]
                k = j
            else:
                k += 2

    def is_data(v):
        """data address? combined: any value in the data or bss; separate: only a known name or the data of a module
        (a small value is a constant or a field offset, not an address)"""
        if not separate:
            return dbase <= v < bend
        return v in names or any(m["data"] and m["dstart"] <= v < m["dstart"] + m["data"] for m in mods)

    def classify(t):
        res = []
        if t.startswith(".word"):
            return res
        branch = t.split("\t")[0] in ("jr", "jp", "djnz", "dbjnz", "calr", "call")
        for h in HEX.finditer(t):
            if len(h.group(1)) != 4:
                continue
            v = int(h.group(1), 16)
            before, after = t[:h.start()], t[h.end():]
            imm = before.endswith("#")
            idx = after.startswith("(")
            if branch and not imm and not idx:
                if 0 <= v < tsize:
                    res.append((h, "text", v))
                continue
            if idx:
                b = re.match(r"\((r\d+)\)", after)
                if b and b.group(1) == "r15":
                    continue
                if is_data(v):
                    res.append((h, "data", v))
                elif v >= 0x100 and v in starts:
                    res.append((h, "text", v))
                continue
            if imm:
                if separate:
                    # a small constant and a data address look alike: data only when it is a known name
                    # or inside the data of a module
                    if v in names or any(m["data"] and m["dstart"] <= v < m["dstart"] + m["data"] for m in mods) or (bbase <= v < bend and v in names):
                        res.append((h, "data", v))
                    elif v < tsize and v in names:
                        res.append((h, "text", v))
                    continue
                if dbase <= v < bbase or (bbase <= v < bend and v in names):
                    res.append((h, "data", v))
                elif v < tsize and v in names:
                    res.append((h, "text", v))
                continue
            if separate and not is_data(v) and 0 <= v < tsize and v in names:
                res.append((h, "text", v))
                continue
            if is_data(v):
                res.append((h, "data", v))
            elif 0 <= v < tsize:
                res.append((h, "text", v))
        return res

    problems = []

    def data_name(v, m):
        om = dmodule_of(v)
        if om is m:
            if v in names:
                return names[v], False
            return "D%04x" % (v - m["dstart"]), True
        if v in names:
            return names[v], False
        best = max((a for a in names if a <= v and dbase <= a < bend), default=None)
        if best is not None:
            return "%s+%d" % (names[best], v - best), False
        problems.append("no name for the address %04x (module %s)" % (v, m["name"]))
        return "_x%04x" % v, False

    texts = {}
    for m in mods:
        if m["kind"] != "old":
            continue
        ma, mb = m["start"], m["end"]
        ext, local_labels, body = set(), set(), []
        exported = {a: n for n, a in m["syms"].items() if ma <= a < mb}
        for pc, t in ins:
            if not (ma <= pc < mb):
                continue
            t2 = t
            for h, kind, v in sorted(classify(t), key=lambda x: -x[0].start()):
                if kind == "text":
                    if module_of(v) is m:
                        if v in exported:
                            sym = exported[v]
                        else:
                            sym = "L%04x" % (v - ma)
                            local_labels.add(v)
                    elif v in names:
                        sym = names[v]
                        ext.add(sym)
                    else:
                        problems.append("no name for the text address %04x (%s)" % (v, m["name"]))
                        sym = "_x%04x" % v
                else:
                    sym, loc = data_name(v, m)
                    base = sym.split("+")[0]
                    if not loc and base not in m["comms"] and not (base in m["syms"] and dmodule_of(v) is m):
                        ext.add(base)
                t2 = t2[:h.start()] + sym + t2[h.end():]
            body.append((pc, t2.replace("\t// EPU", "")))
        outl = []
        for n, (a, sz) in m["comms"].items():
            outl.append("\t.comm\t%d, %s" % (sz, n))
        if ext:
            outl.append("\t.extern\t" + ", ".join(sorted(ext)))
        outl.append("\t.psec")
        outl.append("\t.code")
        tab_t = set(w for q, w in tables.items() if dmodule_of(q) is m)
        for pc, t in body:
            if pc in exported:
                outl.append("%s::" % exported[pc])
            elif pc in local_labels or pc in tab_t:
                outl.append("L%04x:" % (pc - ma))
            outl.append("\t" + t)
        if m["data"]:
            outl.append("\t.psec")
            outl.append("\t.data")
            lo, hi = m["dstart"], m["dstart"] + m["data"]
            lab = set()
            for pc, t in ins:
                for h, kind, v in classify(t):
                    if kind == "data" and lo <= v < hi:
                        lab.add(v)
            lab |= set(a for a in names if lo <= a < hi)
            k = lo
            while k < hi:
                if k in lab:
                    outl.append(("%s::" % names[k]) if k in names else "D%04x:" % (k - lo))
                if k in tables:
                    outl.append("\t.addr\tL%04x" % (tables[k] - ma))
                    k += 2
                    continue
                j = k + 1
                while j < hi and j not in lab and j not in tables:
                    j += 1
                chunk = img[dimg(k):dimg(j)]
                for q in range(0, len(chunk), 8):
                    outl.append("\t.byte\t" + ",".join("%%%02x" % c for c in chunk[q:q + 8]))
                k = j
        texts[m["name"]] = "\n".join(outl) + "\n"
    order = []
    for m in mods:
        if m["kind"] == "shipped":
            order.append("shipped %s" % m["name"])
            continue
        d = os.path.join(outdir, m["name"])
        os.makedirs(d, exist_ok=True)
        n = 1
        while True:
            fn = os.path.join(d, "0_%d.s" % n)
            if not os.path.exists(fn):
                open(fn, "w").write(texts[m["name"]])
                break
            if open(fn).read() == texts[m["name"]]:
                break
            n += 1
        order.append("old %s 0_%d" % (m["name"], n))
    # which program has which version of a module
    vf = os.path.join(outdir, "VERSIONS")
    table = {}
    if os.path.exists(vf):
        for line in open(vf):
            f = line.split()
            if len(f) >= 3 and not line.startswith("#"):
                table[(f[0], f[1])] = set(f[2:])
    prog_name = os.path.basename(path)
    for line in order:
        f = line.split()
        if f[0] == "old":
            table.setdefault((f[1], f[2]), set()).add(prog_name)
    with open(vf, "w") as fh:
        fh.write("# module  version  programs that contain this version (made by host/tools/unlink.py)\n")
        for (n, v), ps in sorted(table.items()):
            fh.write("%-10s %-5s %s\n" % (n, v, " ".join(sorted(ps))))
    print("\n".join(order))
    for p in sorted(set(problems)):
        print("PROBLEM:", p, file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
