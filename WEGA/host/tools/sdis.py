#!/usr/bin/env python3
"""Symbolic disassembler for WEGA s.out objects (with relocation and symbols).

Usage: sdis.py [-l] file.o
  -l   listing: address and code words in front of each line
  -a   assembler source: no .extern line (symbols in order of first use)

Output follows cas syntax as far as known (see KNOWLEDGE.md). Relocated
operands are shown as symbol+addend. Internal targets without a symbol get
labels L<seg>_<offset>. Text is decoded by linear sweep; data is shown as
.word/.byte lines.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(__file__))
from sout import SOut  # noqa: E402
import z8k  # noqa: E402

AREA = {1: "text", 2: "data", 3: "bss"}


class Obj:
    def __init__(self, fn):
        self.s = s = SOut(open(fn, "rb").read())
        self.seg = (s.magic >> 8) == 0xE6
        # image offset of each (segindex, area)
        self.base = {}
        for si, kind, off, size in s.section_ranges():
            self.base[(si, kind)] = (off, size)
        self.reloc = {}
        if s.reloc:
            for a in range(0, len(s.reloc), 2):
                w = struct.unpack(">H", s.reloc[a:a + 2])[0]
                if w:
                    self.reloc[a] = w
        # symbols: name lookup for (segindex, area, offset)
        self.labels = {}
        self.syms = s.symbols
        for y in s.symbols:
            t = y["type"] & 0x1F
            if t in (2, 3, 4):
                area = {2: "text", 3: "data", 4: "bss"}[t]
                key = (y["segt"], area, y["value"] & 0xFFFF)
                self.labels.setdefault(key, []).append(y)

    def label(self, si, area, off):
        ys = self.labels.get((si, area, off))
        if ys:
            return ys[0]["name"]
        name = "L%d%s_%04x" % (si, area[0], off)
        self.labels[(si, area, off)] = [dict(name=name, type=0, synth=True)]
        return name

    def word(self, imgoff):
        return struct.unpack(">H", self.s.image[imgoff:imgoff + 2])[0]

    def target_of(self, rw, imgoff, stored):
        """Name + addend for relocation word rw on a word with value `stored`."""
        if rw & 8:
            i = rw >> 4
            y = self.s.symidx.get(i)
            return (y["name"] if y else "?sym%d" % i), stored
        area = AREA.get((rw >> 1) & 3)
        if area is None:
            return None, stored
        si = (rw >> 8) if rw & 1 else 0
        if area == "bss":
            return self.label(si, "bss", stored), 0
        return self.label(si, area, stored), 0


def fmt_sym(name, add):
    if add == 0:
        return name
    if add & 0x8000:
        return "%s-%d" % (name, 0x10000 - add)
    return "%s+%d" % (name, add)


def render_op(obj, ins, o, off):
    """Operand text with symbols. off = image offset of the instruction."""
    seg = ins.seg
    k = o.kind

    def rel_at(widx):
        return obj.reloc.get(off + 2 * widx)

    if k in ("da", "x"):
        s_, a_off, form = o.a
        w = o.word
        name = None
        if form == "l":
            r1, r2 = rel_at(w), rel_at(w + 1)
            if r2 or r1:
                name, add = obj.target_of(r2 or r1, off + 2 * w + 2, a_off)
        elif form == "s":
            r = rel_at(w)
            if r:
                name, add = obj.target_of(r, off + 2 * w, a_off)
        else:
            r = rel_at(w)
            if r:
                name, add = obj.target_of(r, off + 2 * w, a_off)
        if name:
            t = fmt_sym(name, add)
            if form == "s":
                t = "|%s|" % t
        else:
            t = z8k.addr_text(o.a, seg)
        return t + ("(r%d)" % o.b if k == "x" else "")
    if k == "imm" and o.word is not None and o.word > 0:
        if o.b == 32:
            r = rel_at(o.word + 1) or rel_at(o.word)
            if r:
                name, add = obj.target_of(r, 0, o.a & 0xFFFF)
                if name:
                    return "#" + fmt_sym(name, add)
        else:
            r = rel_at(o.word)
            if r:
                name, add = obj.target_of(r, 0, o.a)
                if name:
                    return "#" + fmt_sym(name, add)
    if k == "ba":
        r = rel_at(o.word)
        if r:
            name, add = obj.target_of(r, 0, o.b)
            bs, bn = o.a
            return "%s%d(#%s)" % ("rr" if bs == "l" else "r", bn, fmt_sym(name, add))
    if k == "rel":
        r = rel_at(o.word) if o.word is not None else None
        if r and r & 8:
            y = obj.s.symidx.get(r >> 4)
            return y["name"] if y else "?sym%d" % (r >> 4)
        return None  # internal: filled by caller with a label
    return z8k.op_text(o, ins)


NOEXT = False


def disasm(fn, listing):
    obj = Obj(fn)
    s = obj.s
    out = []
    # pass 1: decode text, collect internal branch targets
    decoded = {}
    for si, kind, off, size in s.section_ranges():
        if kind != "text":
            continue
        buf = z8k.Buf(s.image[off:off + size])
        pc, lst = 0, []
        while pc < size:
            ins = z8k.decode(buf, pc, obj.seg)
            if ins is None:
                lst.append((pc, None))
                pc += 2
                continue
            for o in ins.ops:
                if o.kind == "rel":
                    r = obj.reloc.get(off + pc + 2 * (o.word or 0))
                    if not (r and r & 8):
                        obj.label(si, "text", o.a)
            lst.append((pc, ins))
            pc = ins.end
        decoded[si] = (off, lst)

    for _pass in (0, 1):   # pass 0 only creates the labels needed by forward references
        out = []
        out.append("// %s: magic %04X %s" % (os.path.basename(fn), s.magic, "segmented" if obj.seg else "nonsegmented"))
        if obj.seg:
            out.append("\t.seg")
        ext = [y["name"] for y in s.symbols if (y["type"] & 0x1F) == 0 and y["type"] & 0x20]
        comm = [(y["name"], y["value"]) for y in s.symbols if (y["type"] & 0x1F) == 0 and y["value"]]
        if NOEXT:
            # assembler source: declare all symbols in symbol table order so
            # that cas rebuilds the identical symbol table
            for y in s.symbols:
                t = y["type"]
                if t & 0x1F == 0 and y["value"]:
                    out.append("\t.comm\t%d, %s" % (y["value"], y["name"]))
                elif t & 0x1F == 0 or t & 0x20:
                    out.append("\t.extern\t" + y["name"])
        else:
            if ext:
                out.append("\t.extern\t" + ", ".join(n for n in ext if n not in dict(comm)))
            for n, v in comm:
                out.append("\t.comm\t%d, %s" % (v, n))

        def labels_at(si, area, a):
            res = []
            for y in obj.labels.get((si, area, a), []):
                glob = (y.get("type", 0) & 0x20) != 0
                res.append("%s%s" % (y["name"], "::" if glob else ":"))
            return res

        for si, kind, off, size in s.section_ranges():
            out.append("")
            out.append("// segment table entry %d (segment %d), %s, %d bytes" % (
                si, s.segs[si]["segno"], kind, size))
            out.append("\t.psec\n\t.%s" % ("code" if kind == "text" else "data"))
            if kind == "text":
                for pc, ins in decoded[si][1]:
                    for lab in labels_at(si, "text", pc):
                        out.append(lab)
                    if ins is None:
                        w = obj.word(off + pc)
                        line = "\t.word\t%s\t// undecodable" % z8k.hx(w)
                        words = [w]
                    else:
                        ops = []
                        for o in ins.ops:
                            t = render_op(obj, ins, o, off + pc)
                            if t is None:
                                t = obj.label(si, "text", o.a)
                            ops.append(t)
                        ops = [x for x in ops if x != ""]
                        if ins.mn == "epu":
                            line = "\t" + z8k.text(ins)
                        else:
                            line = "\t" + ins.mn + ("\t" + ",".join(ops) if ops else "")
                        words = ins.words
                    if listing:
                        line = "%04x %-19s%s" % (pc, " ".join("%04x" % w for w in words), line)
                    out.append(line)
            else:
                a = 0
                while a < size:
                    for lab in labels_at(si, "data", a):
                        out.append(lab)
                    r = obj.reloc.get(off + a)
                    w = obj.word(off + a) if a + 2 <= size else s.image[off + a] << 8
                    if r:
                        name, add = obj.target_of(r, off + a, w)
                        line = "\t.word\t%s" % fmt_sym(name, add)
                    elif a + 2 <= size:
                        line = "\t.word\t%s" % z8k.hx(w)
                    else:
                        line = "\t.byte\t%s" % z8k.hx(w >> 8, 2)
                    if listing:
                        line = "%04x %-19s%s" % (a, "%04x" % w, line)
                    out.append(line)
                    a += 2
        # bss symbols
        bss = [y for y in s.symbols if (y["type"] & 0x1F) == 4]
        if bss:
            out.append("\n\t.bss")
            for y in sorted(bss, key=lambda y: y["value"]):
                out.append("%s%s\t// bss at %04x" % (y["name"], "::" if y["type"] & 0x20 else ":",
                                                    y["value"] & 0xFFFF))
    return "\n".join(out)


def main(argv):
    global NOEXT
    listing = "-l" in argv
    NOEXT = "-a" in argv
    for fn in [a for a in argv if not a.startswith("-")]:
        print(disasm(fn, listing))


if __name__ == "__main__":
    try:
        main(sys.argv[1:])
    except BrokenPipeError:
        pass
