#!/usr/bin/env python3
"""oldlibc_match.py OBJDIR PROGRAM - which oldlibc modules occur byte-identically (instruction for instruction, relocated
values masked, as libcver.py) in PROGRAM.

OBJDIR holds the assembled modules NAME_0_n.o (cas -u of src/lib/oldlibc/NAME/0_n.s, the output of cas is a.out and
must be renamed). For each module: the first instruction index in PROGRAM and the length; a module that occurs
completely is a byte-identical version. Partial occurrences (the module's own run in PROGRAM) are reported as well.
"""
import glob
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'tools'))
import libcver  # noqa: E402
from sout import SOut  # noqa: E402


def module_seq(path):
    s = SOut(open(path, 'rb').read())
    seg = (s.magic >> 8) == 0xE6
    img = bytearray(s.image)
    for a in range(0, len(s.reloc) - 1, 2):
        if s.reloc[a] or s.reloc[a + 1]:
            img[a:a + 2] = b'\xff\xff'
    return [libcver.norm(t) for p, t in libcver.decode(bytes(img), s.segs[0]['code'], seg)]


def main():
    objdir, prog = sys.argv[1], sys.argv[2]
    _, pins, _ = libcver.load_program(prog)
    texts = [t for p, t in pins]
    mods = {}
    for f in sorted(glob.glob(os.path.join(objdir, '*.o'))):
        mods[os.path.basename(f)[:-2]] = module_seq(f)
    idx = {}
    for i in range(len(texts) - 2):
        idx.setdefault((texts[i], texts[i + 1], texts[i + 2]), []).append(i)
    for name in sorted(mods):
        seq = mods[name]
        if len(seq) < 3:
            print('%-16s too short' % name)
            continue
        hits = [i for i in idx.get((seq[0], seq[1], seq[2]), []) if texts[i:i + len(seq)] == seq]
        if hits:
            print('%-16s IDENTICAL at %04x (%d instructions)' % (name, pins[hits[0]][0], len(seq)))
        else:
            print('%-16s no full occurrence (%d instructions)' % (name, len(seq)))


if __name__ == '__main__':
    main()
