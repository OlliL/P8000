#!/usr/bin/env python3
"""oldslibc_cut.py - cut the libc part of /bin/sld into one assembler source per libc object, and the own code as sld_own.s.

Usage: oldslibc_cut.py OLDSLIBC_DIR SLD_OWN.s [VERSION]

The libc part of sld (0x8682..0x9730) is a sequence of members of slibc.a. The boundaries are the sizes of the members
(checked: the code of every object equals the member of /lib/slibc.a with the relocated words masked, `cmp_shipped()`),
the names of the entry points are the global text symbols of the members. Every object gets the directory
OLDSLIBC_DIR/<object>/<VERSION>.s (VERSION default 0_1: an invented revision number, other programs will carry other
revisions of the same objects). ffltioc, getpid and umask (6 to 8 bytes) are not cut: the Makefile takes them from slibc.a.
The own code (0x56..0x8682, the start code 0x18..0x55 is scrt0.s) is written to SLD_OWN.s with calls into the libc part
resolved to the same names. Data: run libc_data_seg.py on OLDSLIBC_DIR afterwards.
"""
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import unlink_seg as U  # noqa: E402
from sout import SOut  # noqa: E402

EMU = '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu'
WEGA = '/mnt/share/cvs/P8000/tmp/wega31_root'
PROG = os.path.join(WEGA, 'bin', 'sld')
SLIBC = os.path.join(WEGA, 'lib', 'slibc.a')

# (object, start, end, shipped): in link order; sizes = sizes of the members of slibc.a
OBJECTS = [('ffltioc', 0x8682, 0x8688, True), ('ungetc', 0x8688, 0x86fa, False), ('filbuf', 0x86fa, 0x880e, False),
           ('flsbuf', 0x880e, 0x8a48, False), ('__callex', 0x8a48, 0x8a50, False), ('cuexit', 0x8a50, 0x8a5c, False),
           ('atoi', 0x8a5c, 0x8ad0, False), ('cnvt', 0x8ad0, 0x8be8, False), ('decimal', 0x8be8, 0x8c86, False),
           ('length', 0x8c86, 0x8cb4, False), ('malloc', 0x8cb4, 0x91ea, False), ('isatty', 0x91ea, 0x9210, False),
           ('mktemp', 0x9210, 0x9296, False), ('stty', 0x9296, 0x92c6, False), ('strchr', 0x92c6, 0x92f8, False),
           ('strrchr', 0x92f8, 0x932e, False), ('swab', 0x932e, 0x9376, False), ('rindex', 0x9376, 0x93ac, False),
           ('strcat', 0x93ac, 0x93e8, False), ('strncmp', 0x93e8, 0x943c, False), ('strcpy', 0x943c, 0x946e, False),
           ('swap', 0x946e, 0x952c, False), ('access', 0x952c, 0x953e, False), ('tolower', 0x953e, 0x9564, False),
           ('chmod', 0x9564, 0x9576, False), ('close', 0x9576, 0x9586, False), ('creat', 0x9586, 0x9596, False),
           ('fstat', 0x9596, 0x95aa, False), ('getpid', 0x95aa, 0x95b0, True), ('ioctl', 0x95b0, 0x95c8, False),
           ('link', 0x95c8, 0x95da, False), ('lseek', 0x95da, 0x95ee, False), ('mkseg', 0x95ee, 0x95fe, False),
           ('open', 0x95fe, 0x9610, False), ('read', 0x9610, 0x9622, False), ('ssgbrk', 0x9622, 0x9638, False),
           ('signal', 0x9638, 0x96f2, False), ('umask', 0x96f2, 0x96fa, True), ('unlink', 0x96fa, 0x970a, False),
           ('write', 0x970a, 0x971c, False), ('cerror', 0x971c, 0x9730, False)]
OWN = (0x56, 0x8682)
LIBC_LO, LIBC_HI = 0x8682, 0x9730


def extract_members(tmp):
    shutil.copy(SLIBC, os.path.join(tmp, 'slibc.a'))
    names = [o + '.o' for o, _, _, _ in OBJECTS]
    subprocess.run([EMU, '-r', WEGA, '/bin/ar', 'x', 'slibc.a'] + names, cwd=tmp, check=True, capture_output=True)


def member_symbols(tmp, obj, a):
    """global text symbols of the member: {address: [names]}"""
    o = SOut(open(os.path.join(tmp, obj + '.o'), 'rb').read())
    out = {}
    for sy in o.symbols:
        if (sy['type'] & 0x3f) == 34 and sy['segt'] == 0:
            out.setdefault(a + sy['value'], []).append(sy['name'])
    return out


def cmp_shipped(tmp, prog, obj, a, b):
    o = SOut(open(os.path.join(tmp, obj + '.o'), 'rb').read())
    code = bytes(o.image[:o.segs[0]['code']])
    ref = prog[a:b]
    if len(code) != len(ref):
        return -1
    d = 0
    for k in range(0, len(code), 2):
        masked = any(o.reloc[k + j] for j in (0, 1)) if k + 1 < len(o.reloc) else False
        if not masked and code[k:k + 2] != ref[k:k + 2]:
            d += 1
    return d


def main():
    outdir, ownfile = sys.argv[1], sys.argv[2]
    ver = sys.argv[3] if len(sys.argv) > 3 else '0_1'
    s = SOut(open(PROG, 'rb').read())
    prog = bytes(s.image[0:s.segs[0]['code']])
    seg = (s.magic >> 8) == 0xE6
    tmp = tempfile.mkdtemp(prefix='oldslibc', dir='/mnt/share/cvs/P8000/tmp/tmp')
    extract_members(tmp)
    syms = {}
    for obj, a, b, _ in OBJECTS:
        d = cmp_shipped(tmp, prog, obj, a, b)
        print('%-9s %04x-%04x  against the member of slibc.a: %s' % (obj, a, b, 'equal' if d == 0 else 'DIFFERENT (%d)' % d))
        if d != 0:
            sys.exit(1)
        for addr, ns in member_symbols(tmp, obj, a).items():
            syms.setdefault(addr, []).extend(ns)
    extnames = {'0x%x' % t: ns[0] for t, ns in syms.items()}
    work = os.path.join(tmp, 'work')
    os.makedirs(work)
    for obj, a, b, shipped in OBJECTS:
        if shipped:
            continue
        exp = {t: ns for t, ns in syms.items() if a <= t < b}
        first = exp[a][0]
        src = U.write_source(prog, seg, a, b, first, work,
                             extnames, exports=exp)
        diff, info = U.verify(prog, seg, a, b, src, work)
        print('%-9s verify: differing words %s, %s' % (obj, diff, info))
        if diff != 0:
            sys.exit(1)
        d = os.path.join(outdir, obj)
        os.makedirs(d, exist_ok=True)
        shutil.copy(src, os.path.join(d, ver + '.s'))
    # the own code: calls into the libc part by name, the entry of main is exported
    a, b = OWN
    src = U.write_source(prog, seg, a, b, 'sld_own', work, extnames, exports={0x78c: ['main']})
    diff, info = U.verify(prog, seg, a, b, src, work)
    print('sld_own   verify: differing words %s, %s' % (diff, info))
    text = open(src).read().replace('_sld_own::\n', '', 1)
    open(ownfile, 'w').write(text)


if __name__ == '__main__':
    main()
