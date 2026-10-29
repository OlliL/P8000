#!/usr/bin/env python3
"""ldcheck.py - compile a C draft of /bin/ld with the original cc (emulator) and compare its code with /bin/ld.

Usage: ldcheck.py DRAFT.c LO HI [NAME]

  DRAFT.c  the draft: one or more K&R functions (no prototypes); it is compiled alone with '/bin/cc -O -c'
  LO HI    code range of the original (hex), e.g. 22e8 2360; the function starts at LO
  NAME     the symbol of the function in the object (default: the name of the first function of the draft)

Model: src/cmd/sld/tools/fncheck.py (non-segmented here: decode seg=0, the compiler is /bin/cc, the original is /bin/ld).
Addresses are masked, frame offsets (...(r15)) and local frame addresses are compared. Prints OK or the differing
instructions (object | original).
"""
import difflib
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
WEGA = os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root')
EMU = os.environ.get('EMU', '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu')
ORIG = os.path.join(WEGA, 'bin', 'ld')
sys.path.insert(0, os.path.join(HERE, '..', '..', '..', '..', 'host', 'tools'))
import z8k  # noqa: E402
from sout import SOut  # noqa: E402


def insns(code, lo, hi):
    buf = z8k.Buf(code, 0)
    out, pc = [], lo
    while pc < hi:
        i = z8k.decode(buf, pc, 0)
        if i is None:
            out.append((pc, '.word %04x' % buf.w(pc)))
            pc += 2
            continue
        out.append((pc, z8k.text(i)))
        pc = i.end
    return out


def norm(t):
    keep = []

    def hold(m):
        keep.append(m.group(0))
        return '\0%d\0' % (len(keep) - 1)
    t = re.sub(r'%[0-9A-Fa-f]{1,4}\(r15\)', hold, t)
    t = re.sub(r'%[0-9A-Fa-f]{3,}', 'ADDR', t)
    t = re.sub(r'\0(\d+)\0', lambda m: keep[int(m.group(1))], t)
    return re.sub(r'^calr\t', 'call\t', t)


def compile_obj(src):
    tmp = tempfile.mkdtemp(prefix='ldcheck', dir='/mnt/share/cvs/P8000/tmp/tmp')
    base = os.path.splitext(os.path.basename(src))[0]
    shutil.copy(src, os.path.join(tmp, base + '.c'))
    r = subprocess.run([EMU, '-r', WEGA, '/bin/cc', '-O', '-c', base + '.c'], cwd=tmp,
                       capture_output=True, text=True, timeout=600)
    if r.returncode != 0 or not os.path.exists(os.path.join(tmp, base + '.o')):
        return None, (r.stdout + r.stderr).strip()
    return os.path.join(tmp, base + '.o'), ''


def main():
    src, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
    path, err = compile_obj(src)
    if path is None:
        sys.exit('does not compile:\n' + err)
    o = SOut(open(path, 'rb').read())
    orig = SOut(open(ORIG, 'rb').read())
    ocode = o.image[:o.segs[0]['code']]
    mine = insns(ocode, 0, len(ocode))
    theirs = insns(orig.image[:orig.segs[0]['code']], lo, hi)
    a = [norm(t) for _, t in mine]
    b = [norm(t) for _, t in theirs]
    if a == b:
        print('OK: %d instructions equal (%04x..%04x)' % (len(b), lo, hi))
        return
    print('DIFF: object %d instructions, original %d' % (len(a), len(b)))
    for line in difflib.unified_diff(b, a, 'original', 'object', lineterm='', n=2):
        print(line)


if __name__ == '__main__':
    main()
