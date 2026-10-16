#!/usr/bin/env python3
"""Decoder self-check: linear sweep over the text sections of s.out objects
and verify that every relocated word falls on an operand word of a decoded
instruction with a fitting role.

Usage: check_reloc.py [-v] obj ...
"""
import struct
import sys
import os

sys.path.insert(0, os.path.dirname(__file__))
from sout import SOut  # noqa: E402
import z8k  # noqa: E402

# which instruction word roles may carry which relocation kind
OK_ROLES = {
    0: {"addr", "offw", "imm", "imm32l", "disp16", "port"},   # 16 bit value / offset
    1: {"segw", "imm32h"},                                    # segment part
    2: {"addr"},                                              # short segmented addr
    3: {"op"},                                                # 12 bit rel (CALR)
    4: {"disp16"},                                            # 16 bit rel (LDR/LDAR)
}


def kind_of(w):
    if w & 8:
        return w & 7
    if w & 1:
        return (w >> 4) & 7
    return 0


def check(fn, verbose=False):
    s = SOut(open(fn, "rb").read())
    seg = (s.magic >> 8) == 0xE6
    stats = dict(insns=0, bad=0, rel=0, relbad=0)
    for si, kind, off, size in s.section_ranges():
        if kind != "text":
            continue
        code = s.image[off:off + size]
        rel = s.reloc[off:off + size] if s.reloc else b""
        relmap = {}
        for a in range(0, len(rel), 2):
            w = struct.unpack(">H", rel[a:a + 2])[0]
            if w:
                relmap[a] = w
        buf = z8k.Buf(code)
        pc = 0
        covered = {}
        while pc < size:
            ins = z8k.decode(buf, pc, seg)
            if ins is None:
                stats["bad"] += 1
                if verbose:
                    print("%s: seg%d %04x: undecodable %04x" % (fn, si, pc, buf.w(pc)))
                covered[pc] = ("baddata", None)
                pc += 2
                continue
            stats["insns"] += 1
            for k, role in enumerate(ins.roles):
                covered[pc + 2 * k] = (role, ins)
            pc = ins.end
        for a, w in sorted(relmap.items()):
            stats["rel"] += 1
            role, ins = covered.get(a, ("none", None))
            k = kind_of(w)
            if role not in OK_ROLES.get(k, ()):
                stats["relbad"] += 1
                if verbose:
                    t = z8k.text(ins) if ins else "-"
                    print("%s: seg%d %04x: reloc %04x kind %d on role %s  [%04x: %s]" % (
                        fn, si, a, w, k, role, ins.pc if ins else 0, t))
    return stats


def main(argv):
    verbose = "-v" in argv
    files = [a for a in argv if a != "-v"]
    tot = dict(insns=0, bad=0, rel=0, relbad=0)
    for fn in files:
        try:
            st = check(fn, verbose)
        except ValueError as e:
            print("%s: %s" % (fn, e))
            continue
        for k in tot:
            tot[k] += st[k]
        if st["bad"] or st["relbad"]:
            print("%-40s insns %5d bad %3d rel %4d relbad %3d" % (
                fn, st["insns"], st["bad"], st["rel"], st["relbad"]))
    print("TOTAL insns %d undecodable %d relocs %d misplaced %d" % (
        tot["insns"], tot["bad"], tot["rel"], tot["relbad"]))


if __name__ == "__main__":
    main(sys.argv[1:])
