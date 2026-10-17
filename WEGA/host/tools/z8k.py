#!/usr/bin/env python3
"""Z8000 (U8001/U8002) instruction decoder, segmented and non-segmented.

decode(mem, pc, seg) -> Insn or None (None = not a valid instruction).
`mem` is any object with .w(addr) returning the 16-bit big-endian word at
byte address `addr` (or raising IndexError past the end).

Each Insn records, per word, what the word is (role), so that relocation
entries of s.out objects can be matched to operands.

Encoding sources: Z8000 CPU manuals (Zilog, SGS-Thomson, U8000), checked
against relocation positions in WEGA objects. EPU (Z8070) instructions are
decoded only for their length and shown as raw words.
"""

COND = ["f", "lt", "le", "ule", "ov", "mi", "eq", "c",
        "t", "ge", "gt", "ugt", "nov", "pl", "ne", "nc"]
# cas names for the flag operands of setflg/resflg/comflg (bit order C Z S P/V)
FLAGBITS = [(8, "c"), (4, "z"), (2, "s"), (1, "v")]
CTRL = {2: "fcw", 3: "refresh", 4: "psapseg", 5: "psapoff", 6: "nspseg", 7: "nspoff"}


class Op:
    """One operand. kind is one of:
    reg(size,n) ir(n) iio(n) imm(val,bits) da(addr) x(addr,idx) ba(base,disp)
    bx(base,idx) rel(target) cc(n) flags(n) ints(n) ctrl(name) bit(n)
    cnt(n) port(val) flagsreg
    For da/x: addr is (seg, off, form) with form 'n' (nonseg), 's' short, 'l' long.
    `word` is the index of the first word that carries the value (or None).
    """
    __slots__ = ("kind", "a", "b", "word")

    def __init__(self, kind, a=None, b=None, word=None):
        self.kind, self.a, self.b, self.word = kind, a, b, word

    def __repr__(self):
        return "Op(%s,%r,%r,w%s)" % (self.kind, self.a, self.b, self.word)


class Insn:
    __slots__ = ("pc", "words", "roles", "mn", "ops", "seg", "note")

    def __init__(self, pc, seg):
        self.pc, self.seg = pc, seg
        self.words, self.roles, self.ops = [], [], []
        self.mn, self.note = None, None

    @property
    def size(self):
        return 2 * len(self.words)

    @property
    def end(self):
        return self.pc + self.size


class Bad(Exception):
    pass


class _D:
    """Decoder state for one instruction."""

    def __init__(self, mem, pc, seg):
        self.mem, self.seg = mem, seg
        self.i = Insn(pc, seg)
        self.fetch("op")

    def fetch(self, role):
        w = self.mem.w(self.i.pc + self.i.size)
        self.i.words.append(w)
        self.i.roles.append(role)
        return w

    @property
    def w0(self):
        return self.i.words[0]

    def nib(self, k):  # nibble k of the first two words (0 = high nibble of w0)
        w = self.i.words[k // 4]
        return (w >> (12 - 4 * (k % 4))) & 0xF

    # operand builders -------------------------------------------------
    def reg(self, size, n):
        if size == "b":
            return Op("reg", "b", n)
        if size == "l" and n & 1:
            raise Bad("odd rr")
        if size == "q" and n & 3:
            raise Bad("bad rq")
        return Op("reg", size, n)

    def ir(self, n):
        if n == 0:
            raise Bad("ir r0")
        if self.seg and n & 1:
            raise Bad("odd seg ir")
        return Op("ir", n)

    def addr(self, idx=None):
        """direct address (DA) or indexed (X) in the following word(s)."""
        k = len(self.i.words)
        w = self.fetch("addr")
        if not self.seg:
            a = (0, w, "n")
        elif w & 0x8000:
            if w & 0xFF:
                raise Bad("long seg addr low byte")
            self.i.roles[-1] = "segw"
            off = self.fetch("offw")
            a = ((w >> 8) & 0x7F, off, "l")
        else:
            a = ((w >> 8) & 0x7F, w & 0xFF, "s")
        if idx:
            return Op("x", a, idx, k)
        return Op("da", a, None, k)

    def da_or_x(self, n):
        return self.addr(n if n else None)

    def imm(self, bits):
        k = len(self.i.words)
        if bits == 8:
            w = self.fetch("imm")
            return Op("imm", w & 0xFF, 8, k)
        if bits == 16:
            return Op("imm", self.fetch("imm"), 16, k)
        hi = self.fetch("imm32h")
        lo = self.fetch("imm32l")
        return Op("imm", (hi << 16) | lo, 32, k)

    def basereg(self, n):
        if n == 0:
            raise Bad("base r0")
        if self.seg:
            if n & 1:
                raise Bad("odd seg base")
            return ("l", n)
        return ("w", n)

    def rel16(self):
        k = len(self.i.words)
        d = self.fetch("disp16")
        if d & 0x8000:
            d -= 0x10000
        return Op("rel", None, d, k)  # target filled in by finish()

    def finish(self):
        end = self.i.end
        for o in self.i.ops:
            if o.kind == "rel" and o.a is None:
                o.a = (end + o.b) & 0xFFFF
        return self.i


# --- tables --------------------------------------------------------------

# two operand, register destination (nibble 3), source by mode:
# base: (mnemonic, dst size, src reg size, immediate bits or None)
ALU = {
    0x00: ("addb", "b", "b", 8), 0x01: ("add", "w", "w", 16),
    0x02: ("subb", "b", "b", 8), 0x03: ("sub", "w", "w", 16),
    0x04: ("orb", "b", "b", 8), 0x05: ("or", "w", "w", 16),
    0x06: ("andb", "b", "b", 8), 0x07: ("and", "w", "w", 16),
    0x08: ("xorb", "b", "b", 8), 0x09: ("xor", "w", "w", 16),
    0x0A: ("cpb", "b", "b", 8), 0x0B: ("cp", "w", "w", 16),
    0x10: ("cpl", "l", "l", 32), 0x12: ("subl", "l", "l", 32),
    0x14: ("ldl", "l", "l", 32), 0x16: ("addl", "l", "l", 32),
    0x18: ("multl", "q", "l", 32), 0x19: ("mult", "l", "w", 16),
    0x1A: ("divl", "q", "l", 32), 0x1B: ("div", "l", "w", 16),
    0x20: ("ldb", "b", "b", 8), 0x21: ("ld", "w", "w", 16),
    0x2C: ("exb", "b", "b", None), 0x2D: ("ex", "w", "w", None),
}
# stores: address destination, register source (nibble 3)
STORE = {0x2E: ("ldb", "b"), 0x2F: ("ld", "w"), 0x1D: ("ldl", "l")}
BITOP = {0x22: "resb", 0x23: "res", 0x24: "setb", 0x25: "set", 0x26: "bitb", 0x27: "bit"}
INCDEC = {0x28: "incb", 0x29: "inc", 0x2A: "decb", 0x2B: "dec"}
GRP_B = {0: "comb", 1: "cpb", 2: "negb", 4: "testb", 5: "ldb", 6: "tsetb", 8: "clrb"}
GRP_W = {0: "com", 1: "cp", 2: "neg", 4: "test", 5: "ld", 6: "tset", 8: "clr", 9: "push"}
SHIFT_B = {0x0: ("rlb", 1), 0x2: ("rlb", 2), 0x4: ("rrb", 1), 0x6: ("rrb", 2),
           0x8: ("rlcb", 1), 0xA: ("rlcb", 2), 0xC: ("rrcb", 1), 0xE: ("rrcb", 2)}
SHIFT_W = {0x0: ("rl", 1), 0x2: ("rl", 2), 0x4: ("rr", 1), 0x6: ("rr", 2),
           0x8: ("rlc", 1), 0xA: ("rlc", 2), 0xC: ("rrc", 1), 0xE: ("rrc", 2)}
# shifts by immediate: (left name, right name, reg size); dynamic: name
SHIFT_IMM = {(0xB2, 1): ("sllb", "srlb", "b"), (0xB2, 9): ("slab", "srab", "b"),
             (0xB3, 1): ("sll", "srl", "w"), (0xB3, 9): ("sla", "sra", "w"),
             (0xB3, 5): ("slll", "srll", "l"), (0xB3, 0xD): ("slal", "sral", "l")}
SHIFT_DYN = {(0xB2, 3): ("sdlb", "b"), (0xB2, 0xB): ("sdab", "b"),
             (0xB3, 3): ("sdl", "w"), (0xB3, 0xB): ("sda", "w"),
             (0xB3, 7): ("sdll", "l"), (0xB3, 0xF): ("sdal", "l")}
BLKIO = {0x0: ("inir", "ini"), 0x1: ("sinir", "sini"), 0x2: ("otir", "outi"),
         0x3: ("sotir", "souti"), 0x8: ("indr", "ind"), 0x9: ("sindr", "sind"),
         0xA: ("otdr", "outd"), 0xB: ("sotdr", "soutd")}
BLKCP = {0x0: "cpi", 0x4: "cpir", 0x8: "cpd", 0xC: "cpdr",
         0x2: "cpsi", 0x6: "cpsir", 0xA: "cpsd", 0xE: "cpsdr"}
TRANS = {0x0: "trib", 0x2: "trtib", 0x4: "trirb", 0x6: "trtirb",
         0x8: "trdb", 0xA: "trtdb", 0xC: "trdrb", 0xE: "trtdrb"}


def _dst_mode(d, cls, n, size):
    """single operand destination by mode class (0 IR, 1 DA/X, 2 R)."""
    if cls == 0:
        return d.ir(n)
    if cls == 1:
        return d.da_or_x(n)
    return d.reg(size, n)


def _decode(d):
    w0 = d.w0
    hi, lo = w0 >> 8, w0 & 0xFF
    n2, n3 = (w0 >> 4) & 0xF, w0 & 0xF
    cls, base = hi >> 6, hi & 0x3F
    ins = d.i
    ops = ins.ops

    if cls == 3:
        top = hi >> 4
        if top == 0xC:
            ins.mn = "ldb"
            ops += [d.reg("b", (w0 >> 8) & 0xF), Op("imm", lo, 8, 0)]
        elif top == 0xD:
            disp = w0 & 0xFFF
            if disp & 0x800:
                disp -= 0x1000
            ins.mn = "calr"
            ops.append(Op("rel", (ins.pc + 2 - 2 * disp) & 0xFFFF, None, 0))
        elif top == 0xE:
            disp = lo - 256 if lo & 0x80 else lo
            ins.mn = "jr"
            cc = (w0 >> 8) & 0xF
            if cc != 8:
                ops.append(Op("cc", cc))
            ops.append(Op("rel", (ins.pc + 2 + 2 * disp) & 0xFFFF, None, 0))
        else:  # F: djnz / dbjnz
            r = (w0 >> 8) & 0xF
            tgt = (ins.pc + 2 - 2 * (w0 & 0x7F)) & 0xFFFF
            if w0 & 0x80:
                ins.mn = "djnz"
                ops += [d.reg("w", r), Op("rel", tgt, None, 0)]
            else:
                ins.mn = "dbjnz"
                ops += [d.reg("b", r), Op("rel", tgt, None, 0)]
        return

    # ----- regular opcodes
    if base in ALU:
        mn, ds, ss, ib = ALU[base]
        ins.mn = mn
        dst = d.reg(ds, n3)
        if cls == 0:
            if n2 == 0:
                if ib is None:
                    raise Bad("no imm")
                src = d.imm(ib)
            else:
                src = d.ir(n2)
        elif cls == 1:
            src = d.da_or_x(n2)
        else:
            src = d.reg(ss, n2)
        ops += [dst, src]
        return

    if base in STORE and cls != 2:
        mn, rs = STORE[base]
        ins.mn = mn
        dst = d.ir(n2) if cls == 0 else d.da_or_x(n2)
        ops += [dst, d.reg(rs, n3)]
        return

    if base in BITOP:
        mn = BITOP[base]
        sz = "b" if mn.endswith("b") else "w"
        if cls == 0 and n2 == 0:
            w1 = d.fetch("ext")
            if w1 & 0xF0FF:
                raise Bad("dyn bit")
            ins.mn = mn
            ops += [d.reg(sz, (w1 >> 8) & 0xF), d.reg("w", n3)]
            return
        ins.mn = mn
        ops += [_dst_mode(d, cls, n2, sz), Op("bit", n3)]
        return

    if base in INCDEC:
        mn = INCDEC[base]
        ins.mn = mn
        ops += [_dst_mode(d, cls, n2, "b" if mn.endswith("b") else "w"), Op("cnt", n3 + 1)]
        return

    if base in (0x0C, 0x0D):
        byte = base == 0x0C
        if cls == 2:
            if byte:
                t = {0: "comb", 1: "ldctlb1", 2: "negb", 4: "testb", 6: "tsetb",
                     8: "clrb", 9: "ldctlb9"}.get(n3)
                if t is None:
                    raise Bad("8c")
                if t == "ldctlb1":
                    ins.mn = "ldctlb"
                    ops += [d.reg("b", n2), Op("flagsreg")]
                elif t == "ldctlb9":
                    ins.mn = "ldctlb"
                    ops += [Op("flagsreg"), d.reg("b", n2)]
                else:
                    ins.mn = t
                    ops.append(d.reg("b", n2))
                return
            t = {0: "com", 1: "setflg", 2: "neg", 3: "resflg", 4: "test",
                 5: "comflg", 6: "tset", 7: "nop", 8: "clr"}.get(n3)
            if t is None:
                raise Bad("8d")
            ins.mn = t
            if t in ("setflg", "resflg", "comflg"):
                ops.append(Op("flags", n2))
            elif t == "nop":
                if n2:
                    raise Bad("nop")
            else:
                ops.append(d.reg("w", n2))
            return
        tbl = GRP_B if byte else GRP_W
        mn = tbl.get(n3)
        if mn is None or (mn == "push" and cls != 0):
            raise Bad("grp")
        if cls == 0 and n2 == 0:
            raise Bad("grp ir0")
        ins.mn = mn
        if mn == "push":
            ops += [d.ir(n2), d.imm(16)]
            return
        dst = _dst_mode(d, cls, n2, "b" if byte else "w")
        ops.append(dst)
        if n3 in (1, 5):
            ops.append(d.imm(8 if byte else 16))
        return

    if base in (0x11, 0x13):  # pushl / push
        mn = "pushl" if base == 0x11 else "push"
        ins.mn = mn
        sp = d.ir(n2)
        if cls == 0:
            src = d.ir(n3)
        elif cls == 1:
            src = d.da_or_x(n3)
        else:
            src = d.reg("l" if base == 0x11 else "w", n3)
        ops += [sp, src]
        return

    if base in (0x15, 0x17):  # popl / pop
        mn = "popl" if base == 0x15 else "pop"
        ins.mn = mn
        sp = d.ir(n2)
        if cls == 0:
            dst = d.ir(n3)
        elif cls == 1:
            dst = d.da_or_x(n3)
        else:
            dst = d.reg("l" if base == 0x15 else "w", n3)
        ops += [dst, sp]
        return

    if base == 0x1C:
        if cls == 2:
            if n3 != 8:
                raise Bad("9c")
            ins.mn = "testl"
            ops.append(d.reg("l", n2))
            return
        if cls == 0 and n2 == 0:
            raise Bad("1c0")
        if n3 == 8:
            ins.mn = "testl"
            ops.append(d.ir(n2) if cls == 0 else d.da_or_x(n2))
            return
        if n3 in (1, 9):
            w1 = d.fetch("ext")
            if w1 & 0xF0F0:
                raise Bad("ldm w1")
            r, cnt = (w1 >> 8) & 0xF, (w1 & 0xF) + 1
            if r + cnt > 16:
                raise Bad("ldm range")
            m = d.ir(n2) if cls == 0 else d.da_or_x(n2)
            ins.mn = "ldm"
            if n3 == 1:
                ops += [d.reg("w", r), m, Op("cnt", cnt)]
            else:
                ops += [m, d.reg("w", r), Op("cnt", cnt)]
            return
        raise Bad("1c")

    if base == 0x1E:
        if cls == 2:
            if n2:
                raise Bad("ret")
            ins.mn = "ret"
            if n3 != 8:
                ops.append(Op("cc", n3))
            return
        if cls == 0 and n2 == 0:
            raise Bad("jp ir0")
        ins.mn = "jp"
        if n3 != 8:
            ops.append(Op("cc", n3))
        ops.append(d.ir(n2) if cls == 0 else d.da_or_x(n2))
        return

    if base == 0x1F:
        if cls == 2 or n3 or (cls == 0 and n2 == 0):
            raise Bad("call")
        ins.mn = "call"
        ops.append(d.ir(n2) if cls == 0 else d.da_or_x(n2))
        return

    if 0x30 <= base <= 0x37 and cls == 0:
        if base in (0x36,):
            raise Bad("36")
        k = base - 0x30
        size = {0: "b", 1: "w", 2: "b", 3: "w", 4: None, 5: "l", 7: "l"}[k]
        if n2 == 0:
            disp = d.rel16()
            if k == 4:
                ins.mn = "ldar"
                ops += [d.reg("l" if d.seg else "w", n3), disp]
            else:
                ins.mn = {"b": "ldrb", "w": "ldr", "l": "ldrl"}[size]
                r = d.reg(size, n3)
                ops += [disp, r] if k in (2, 3, 7) else [r, disp]
            return
        bs, bn = d.basereg(n2)
        k2 = len(ins.words)
        disp = d.fetch("imm")
        m = Op("ba", (bs, bn), disp, k2)
        if k == 4:
            ins.mn = "lda"
            ops += [d.reg("l" if d.seg else "w", n3), m]
        else:
            ins.mn = {"b": "ldb", "w": "ld", "l": "ldl"}[size]
            r = d.reg(size, n3)
            ops += [m, r] if k in (2, 3, 7) else [r, m]
        return

    if 0x30 <= base <= 0x37 and cls == 1:  # 0x70..0x77: based indexed, lda
        k = base - 0x30
        if k == 6:  # 0x76 lda r, DA/X
            ins.mn = "lda"
            ops += [d.reg("l" if d.seg else "w", n3), d.da_or_x(n2)]
            return
        bs, bn = d.basereg(n2)
        w1 = d.fetch("ext")
        if w1 & 0xF0FF:
            raise Bad("bx w1")
        x = (w1 >> 8) & 0xF
        if x == 0:
            raise Bad("bx x0")
        m = Op("bx", (bs, bn), x)
        size = {0: "b", 1: "w", 2: "b", 3: "w", 4: None, 5: "l", 7: "l"}[k]
        if k == 4:
            ins.mn = "lda"
            ops += [d.reg("l" if d.seg else "w", n3), m]
        else:
            ins.mn = {"b": "ldb", "w": "ld", "l": "ldl"}[size]
            r = d.reg(size, n3)
            ops += [m, r] if k in (2, 3, 7) else [r, m]
        return

    if hi == 0x39 or hi == 0x79:
        if n3:
            raise Bad("ldps")
        ins.mn = "ldps"
        if hi == 0x39:
            ops.append(d.ir(n2))
        else:
            ops.append(d.da_or_x(n2))
        return

    if hi in (0x3A, 0x3B):
        word = hi == 0x3B
        sfx = "" if word else "b"
        if n3 in (4, 5, 6, 7):
            k = len(ins.words)
            port = d.fetch("port")
            r = d.reg("w" if word else "b", n2)
            p = Op("port", port, None, k)
            name = {4: "in", 5: "sin", 6: "out", 7: "sout"}[n3] + sfx
            ins.mn = name
            ops += [r, p] if n3 in (4, 5) else [p, r]
            return
        if n3 not in BLKIO:
            raise Bad("3a")
        w1 = d.fetch("ext")
        if w1 & 0xF000 or (w1 & 0xF) not in (0, 8):
            raise Bad("blkio w1")
        rep, single = BLKIO[n3]
        ins.mn = (rep if (w1 & 0xF) == 0 else single) + sfx
        r = (w1 >> 8) & 0xF
        dn = (w1 >> 4) & 0xF
        if n3 in (0x0, 0x1, 0x8, 0x9):  # input: dst memory n6, src port n2
            ops += [d.ir(dn), Op("iio", n2), d.reg("w", r)]
        else:  # output: dst port n6, src memory n2
            ops += [Op("iio", dn), d.ir(n2), d.reg("w", r)]
        if n2 == 0 or dn == 0:
            raise Bad("blkio r0")
        return

    if 0x3C <= hi <= 0x3F:
        if n2 == 0:
            raise Bad("io r0")
        word = hi & 1
        if hi < 0x3E:
            ins.mn = "in" if word else "inb"
            ops += [d.reg("w" if word else "b", n3), Op("iio", n2)]
        else:
            ins.mn = "out" if word else "outb"
            ops += [Op("iio", n2), d.reg("w" if word else "b", n3)]
        return

    if hi == 0x7A:
        if lo:
            raise Bad("halt")
        ins.mn = "halt"
        return
    if hi == 0x7B:
        if lo == 0x00:
            ins.mn = "iret"
        elif lo == 0x08:
            ins.mn = "mset"
        elif lo == 0x09:
            ins.mn = "mres"
        elif lo == 0x0A:
            ins.mn = "mbit"
        elif n3 == 0xD:
            ins.mn = "mreq"
            ops.append(d.reg("w", n2))
        else:
            raise Bad("7b")
        return
    if hi == 0x7C:
        if lo & 0xF8:
            raise Bad("7c")
        ins.mn = "ei" if lo & 4 else "di"
        ops.append(Op("ints", lo & 3))
        return
    if hi == 0x7D:
        c = n3 & 7
        if c not in CTRL:
            raise Bad("ctrl")
        ins.mn = "ldctl"
        if n3 & 8:
            ops += [Op("ctrl", CTRL[c]), d.reg("w", n2)]
        else:
            ops += [d.reg("w", n2), Op("ctrl", CTRL[c])]
        return
    if hi == 0x7F:
        ins.mn = "sc"
        ops.append(Op("imm", lo, 8, 0))
        return

    if hi == 0xAE or hi == 0xAF:
        ins.mn = "tccb" if hi == 0xAE else "tcc"
        ops += [Op("cc", n3), d.reg("b" if hi == 0xAE else "w", n2)]
        return
    if hi in (0xA0, 0xA1, 0xAC, 0xAD, 0xB4, 0xB5, 0xB6, 0xB7):
        name, sz = {0xA0: ("ldb", "b"), 0xA1: ("ld", "w"), 0xAC: ("exb", "b"),
                    0xAD: ("ex", "w"), 0xB4: ("adcb", "b"), 0xB5: ("adc", "w"),
                    0xB6: ("sbcb", "b"), 0xB7: ("sbc", "w")}[hi]
        ins.mn = name
        ops += [d.reg(sz, n3), d.reg(sz, n2)]
        return
    if hi == 0xB0:
        if n3:
            raise Bad("dab")
        ins.mn = "dab"
        ops.append(d.reg("b", n2))
        return
    if hi == 0xB1:
        if n3 == 0:
            ins.mn = "extsb"
            ops.append(d.reg("w", n2))
        elif n3 == 0xA:
            ins.mn = "exts"
            ops.append(d.reg("l", n2))
        elif n3 == 7:
            ins.mn = "extsl"
            ops.append(d.reg("q", n2))
        else:
            raise Bad("b1")
        return
    if hi in (0xB2, 0xB3):
        rot = SHIFT_B if hi == 0xB2 else SHIFT_W
        if n3 in rot:
            nm, cnt = rot[n3]
            ins.mn = nm
            ops += [d.reg("b" if hi == 0xB2 else "w", n2), Op("cnt", cnt)]
            return
        if (hi, n3) in SHIFT_IMM:
            left, right, sz = SHIFT_IMM[(hi, n3)]
            k = len(ins.words)
            c = d.fetch("imm")
            if c & 0x8000:
                ins.mn = right
                cnt = 0x10000 - c
            else:
                ins.mn = left
                cnt = c
            ops += [d.reg(sz, n2), Op("cnt", cnt, c, k)]
            return
        if (hi, n3) in SHIFT_DYN:
            nm, sz = SHIFT_DYN[(hi, n3)]
            w1 = d.fetch("ext")
            if w1 & 0xF0FF:
                raise Bad("sd w1")
            ins.mn = nm
            ops += [d.reg(sz, n2), d.reg("w", (w1 >> 8) & 0xF)]
            return
        raise Bad("shift")
    if hi == 0xB8:
        if n3 not in TRANS or n2 == 0:
            raise Bad("tr")
        w1 = d.fetch("ext")
        if w1 & 0xF00F:
            raise Bad("tr w1")
        ins.mn = TRANS[n3]
        ops += [d.ir(n2), d.ir((w1 >> 4) & 0xF), d.reg("w", (w1 >> 8) & 0xF)]
        return
    if hi in (0xBA, 0xBB):
        sfx = "" if hi == 0xBB else "b"
        w1 = d.fetch("ext")
        if w1 & 0xF000:
            raise Bad("blk w1")
        r, dn, c = (w1 >> 8) & 0xF, (w1 >> 4) & 0xF, w1 & 0xF
        if n3 in (1, 9):  # ldi/ldir/ldd/lddr
            if c not in (0, 8):
                raise Bad("ld blk")
            name = ("ldi" if n3 == 1 else "ldd") + ("r" if c == 0 else "")
            ins.mn = name + sfx
            ops += [d.ir(dn), d.ir(n2), d.reg("w", r)]
            return
        if n3 not in BLKCP:
            raise Bad("cp blk")
        ins.mn = BLKCP[n3] + sfx
        if n3 & 2:  # string compares: @rd,@rs
            ops += [d.ir(dn), d.ir(n2), d.reg("w", r), Op("cc", c)]
        else:
            ops += [d.reg("w" if hi == 0xBB else "b", dn), d.ir(n2), d.reg("w", r), Op("cc", c)]
        return
    if hi in (0xBC, 0xBE):
        ins.mn = "rrdb" if hi == 0xBC else "rldb"
        ops += [d.reg("b", n3), d.reg("b", n2)]
        return
    if hi == 0xBD:
        ins.mn = "ldk"
        ops += [d.reg("w", n2), Op("imm", n3, 4, 0)]
        return

    if hi in (0x0E, 0x0F, 0x4E, 0x4F, 0x8E, 0x8F):
        # EPU instruction (Z8070 / emulated): template word + 1 word,
        # + address for the DA/X class. Shown raw.
        d.fetch("epu")
        if cls == 1:
            ops.append(d.da_or_x(n2))
        ins.mn = "epu"
        ins.note = "EPU"
        return

    raise Bad("undefined %04x" % w0)


def decode(mem, pc, seg):
    try:
        d = _D(mem, pc, seg)
        _decode(d)
        return d.finish()
    except (Bad, IndexError):
        return None


class Buf:
    """Word access to a bytes object with a base address."""

    def __init__(self, data, base=0):
        self.data, self.base = data, base

    def w(self, a):
        o = a - self.base
        if o < 0 or o + 2 > len(self.data):
            raise IndexError(a)
        return (self.data[o] << 8) | self.data[o + 1]


# --- plain rendering (no symbols) ----------------------------------------

def hx(v, digits=4):
    return "%" + ("%0*X" % (digits, v))


def reg_name(size, n):
    if size == "b":
        return ("rl%d" % (n & 7)) if n & 8 else ("rh%d" % n)
    return {"w": "r", "l": "rr", "q": "rq"}[size] + str(n)


def addr_text(a, seg):
    s, off, form = a
    if form == "n":
        return hx(off)
    if form == "s":
        return "|<<%d>>%s|" % (s, hx(off))
    return "<<%d>>%s" % (s, hx(off))


def op_text(o, ins):
    seg = ins.seg
    k = o.kind
    if k == "reg":
        return reg_name(o.a, o.b)
    if k == "ir":
        return "@" + ("rr" if seg else "r") + str(o.a)
    if k == "iio":
        return "@r%d" % o.a
    if k == "imm":
        if o.b == 4:
            return "#%d" % o.a
        return "#" + hx(o.a, o.b // 4)
    if k == "da":
        return addr_text(o.a, seg)
    if k == "x":
        return "%s(r%d)" % (addr_text(o.a, seg), o.b)
    if k == "ba":
        bs, bn = o.a
        return "%s%d(#%s)" % ("rr" if bs == "l" else "r", bn, hx(o.b))
    if k == "bx":
        bs, bn = o.a
        return "%s%d(r%d)" % ("rr" if bs == "l" else "r", bn, o.b)
    if k == "rel":
        return hx(o.a)
    if k == "cc":
        return COND[o.a]
    if k == "flags":
        return ",".join(n for b, n in FLAGBITS if o.a & b) or "?"
    if k == "ints":
        return {0: "vi,nvi", 1: "vi", 2: "nvi", 3: ""}[o.a]
    if k == "ctrl":
        return o.a
    if k == "flagsreg":
        return "flags"
    if k in ("bit", "cnt"):
        return "#%d" % o.a
    if k == "port":
        return hx(o.a)
    return "?"


def text(ins):
    if ins.mn == "epu":
        return ".word\t" + ",".join(hx(w) for w in ins.words) + "\t// EPU"
    ops = [op_text(o, ins) for o in ins.ops]
    ops = [x for x in ops if x != ""]
    return ins.mn + ("\t" + ",".join(ops) if ops else "")
