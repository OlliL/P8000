#!/usr/bin/env python3
"""Reader for WEGA s.out object files (see a.out(5) of WEGA).

Usage: sout.py [-r] [-x] file.o ...
  -r  print relocation entries
  -x  hex dump text/data per segment
"""
import struct
import sys

MAGICS = {
    0xE607: "seg, I&D combined", 0xE611: "seg, I&D separate", 0xE605: "seg, overlay",
    0xE707: "nonseg, I&D combined", 0xE711: "nonseg, I&D separate", 0xE705: "nonseg, overlay",
    0xE507: "8bit, I&D combined", 0xE511: "8bit, I&D separate", 0xE505: "8bit, overlay",
}
SF = {1: "STRIP", 2: "OPREP", 4: "Z8", 8: "Z80", 0x10: "SEND", 0x20: "7FSTK"}
SG = {1: "CODE", 2: "DATA", 4: "BSS", 8: "STACK", 0x10: "OCODE", 0x20: "ODATA",
      0x40: "OBSS", 0x80: "BOUND"}
NTYPE = {0: "UNDF", 1: "ABS", 2: "CODE", 3: "DATA", 4: "BSS", 0x14: "REG", 0x1E: "SN", 0x1F: "FN"}
RKIND = {0: "off16", 1: "seg", 2: "short", 3: "rel12", 4: "rel16"}


def flags(v, table):
    return "|".join(n for b, n in table.items() if v & b) or "0"


class SOut:
    def __init__(self, data):
        self.data = data
        (self.magic, self.imsize, self.bss, self.segt, self.syms, self.entry,
         self.flag, self.codesz, self.unused) = struct.unpack(">HIIHHIHHH", data[:24])
        if self.magic not in MAGICS:
            raise ValueError("bad magic %04x" % self.magic)
        self.segs = []
        for i in range(self.segt // 16):
            o = 24 + 16 * i
            segno, coff, doff, boff, code, dat, bss, atr, un = struct.unpack(
                ">BBBBHHHhI", data[o:o + 16])
            self.segs.append(dict(segno=segno, coff=coff, doff=doff, boff=boff,
                                  code=code, data=dat, bss=bss, atr=atr))
        self.img_off = 24 + self.segt
        self.image = data[self.img_off:self.img_off + self.imsize]
        stripped = self.flag & 1
        self.rel_off = self.img_off + self.imsize
        self.reloc = b"" if stripped else data[self.rel_off:self.rel_off + self.imsize]
        self.sym_off = self.rel_off + len(self.reloc)
        self.symtab = data[self.sym_off:self.sym_off + self.syms]
        self.trailing = len(data) - (self.sym_off + self.syms)
        self.symbols = self._parse_syms()
        # relocation symbol numbers are 14-byte entry numbers; long names
        # occupy several entries
        self.symidx = {y["idx"]: y for y in self.symbols}

    def _parse_syms(self):
        out, o, st = [], 0, self.symtab
        while o + 14 <= len(st):
            val, typ, segt = struct.unpack(">IBB", st[o:o + 6])
            name = st[o + 6:o + 14]
            idx = o // 14   # relocations count 14-byte entries
            if name[0] & 0x80:
                ln = name[0] & 0x7F           # length incl. length byte
                raw = st[o + 7:o + 6 + ln]
                n = raw.decode("latin-1")
                used = 6 + ln
                o += (used + 13) // 14 * 14    # padded to whole entries
            else:
                n = name.split(b"\0")[0].decode("latin-1")
                o += 14
            out.append(dict(idx=idx, value=val, type=typ, segt=segt, name=n))
        return out

    def section_ranges(self):
        """Yield (segindex, kind, image_offset, size) in file order."""
        o = 0
        for i, s in enumerate(self.segs):
            if s["code"]:
                yield i, "text", o, s["code"]
                o += s["code"]
        for i, s in enumerate(self.segs):
            if s["data"]:
                yield i, "data", o, s["data"]
                o += s["data"]


def symtype(t):
    s = NTYPE.get(t & 0x1F, "%02x" % (t & 0x1F))
    if t & 0x20:
        s += "|EXT"
    if t & 0x40:
        s += "|SEG"
    if t & 0x80:
        s += "|80"
    return s


def reloc_str(w, syms):
    if w & 8:
        k = RKIND.get(w & 7, "?%d" % (w & 7))
        i = w >> 4
        y = syms.get(i)
        n = y["name"] if y else "?sym%d" % i
        return "ext %s %s" % (k, n)
    area = ("abs", "text", "data", "bss")[(w >> 1) & 3]
    if w & 1:
        return "seg st%d %s %s" % (w >> 8, area, RKIND.get((w >> 4) & 7, "?"))
    return "nonseg %s" % area


def main(argv):
    show_rel = "-r" in argv
    show_hex = "-x" in argv
    files = [a for a in argv if not a.startswith("-")]
    for fn in files:
        d = open(fn, "rb").read()
        s = SOut(d)
        print("== %s (%d bytes)" % (fn, len(d)))
        print("magic %04x (%s) imsize %d bss %d segt %d syms %d entry %08x flag %s codesz %d unused %d"
              % (s.magic, MAGICS[s.magic], s.imsize, s.bss, s.segt, s.syms, s.entry,
                 flags(s.flag, SF), s.codesz, s.unused))
        for i, g in enumerate(s.segs):
            print("  seg[%d] no %d coff %d doff %d boff %d code %d data %d bss %d atr %s"
                  % (i, g["segno"], g["coff"], g["doff"], g["boff"], g["code"], g["data"],
                     g["bss"], flags(g["atr"] & 0xFFFF, SG)))
        if s.trailing:
            print("  WARNING: %d trailing bytes" % s.trailing)
        print("  symbols (%d):" % len(s.symbols))
        for y in s.symbols:
            print("    [%3d] %08x %-14s st%-3d %s" % (y["idx"], y["value"], symtype(y["type"]),
                                                    y["segt"], y["name"]))
        if show_rel or show_hex:
            for si, kind, off, size in s.section_ranges():
                print("  %s of seg[%d] at image %#x size %d" % (kind, si, off, size))
                for a in range(off, off + size, 2):
                    w = s.image[a:a + 2]
                    r = s.reloc[a:a + 2] if s.reloc else b"\0\0"
                    rw = struct.unpack(">H", r)[0] if len(r) == 2 else 0
                    if show_hex or rw:
                        print("    %04x: %s%s" % (a - off, w.hex(),
                                                 ("  " + reloc_str(rw, s.symidx)) if rw else ""))


if __name__ == "__main__":
    main(sys.argv[1:])
