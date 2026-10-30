"""zcode.py - instruction lists and the two normal forms of the comparison (used by strictcmp, fncheck, inventory).

  insns(code, lo, hi, seg=False)  (address, text) of the instructions in [lo, hi); undecodable words as '.word'
  norm_masked(t)                  every hex operand of 3+ digits is ADDR, frame operands (r15) kept, calr = call
                                  (the form of the old ldcheck/wholecmp: it hides wrong constants and data addresses)
  at0(t), d18=True               D18 (progcfg D18): @r15 counts as %0000(r15)
  norm_strict(t, codeaddr)        only code addresses are masked: the targets of jr/jp/djnz/call/calr and an immediate
                                  that is in codeaddr (a function address); constants, data, bss and frame offsets stay
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import z8k  # noqa: E402

JUMP = re.compile(r'^(jr|jp|djnz|dbjnz|call|calr)\b')


def insns(code, lo, hi, seg=False):
    sg = 1 if seg else 0
    buf = z8k.Buf(code, 0)
    out, pc = [], lo
    while pc < hi:
        i = z8k.decode(buf, pc, sg)
        if i is None:
            out.append((pc, '.word %04x' % buf.w(pc)))
            pc += 2
            continue
        out.append((pc, z8k.text(i)))
        pc = i.end
    return out


def at0(t):
    """D18: the compiler of cparse writes a frame operand with offset 0 as %0000(r15), the shipped one as @r15"""
    return re.sub(r'@r15\b', '%0000(r15)', t)


def norm_masked(t, d18=False):
    if d18:
        t = at0(t)
    keep = []

    def hold(m):
        keep.append(m.group(0))
        return '\0%d\0' % (len(keep) - 1)
    t = re.sub(r'%[0-9A-Fa-f]{1,4}\(r15\)', hold, t)
    t = re.sub(r'%[0-9A-Fa-f]{3,}', 'ADDR', t)
    t = re.sub(r'\0(\d+)\0', lambda m: keep[int(m.group(1))], t)
    return re.sub(r'^calr\t', 'call\t', t)


def norm_strict(t, codeaddr, low=0x42, d18=False):
    """codeaddr: set of function addresses of the binary the line comes from; values below low (crt0 entries, small
    constants) are never taken as code addresses. d18: @r15 counts as %0000(r15) (see at0)"""
    if d18:
        t = at0(t)
    t = re.sub(r'^calr\t', 'call\t', t)
    if JUMP.match(t):
        return re.sub(r'%[0-9A-Fa-f]{3,8}\b', 'CODE', t)
    return re.sub(r'#%([0-9A-Fa-f]{3,4})\b',
                  lambda m: '#CODE' if int(m.group(1), 16) in codeaddr and int(m.group(1), 16) >= low else m.group(0), t)


def hide_frame(t):
    """frame offsets and the frame size hidden (classification of D1 only)"""
    t = re.sub(r'%[0-9A-Fa-f]{1,4}\(r15\)', 'F(r15)', t)
    return re.sub(r'^((?:dec|inc|sub|add)\tr15,#)\S+$', r'\1N', t)


def hide_regs(t):
    """register names (except r15) hidden (classification of D2 only)"""
    return re.sub(r'\br([rlhq]?)(1[0-4]|[0-9])\b', r'R\1', t)
