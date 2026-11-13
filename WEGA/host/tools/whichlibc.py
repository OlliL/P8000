#!/usr/bin/env python3
"""whichlibc.py PROGRAM OWNEND [LIBDIR]: which library a linked WEGA program was linked from.

Walks through the text of PROGRAM from OWNEND (hex: the end of the own code) and takes at each place the member of a library whose
code fits (addresses masked, the most equal bytes win), for each of LIBDIR/libc-vN.a and LIBDIR/libc/libc.a (default: src/lib of this
tree; make -C host oldroot builds them). Prints per library the bytes of the libc part it covers, best last, and for the best one
the walk: the members in order and each gap with its size, the place of a module this library does not have. A library with no gap is
the one to link: LIBC in the Makefile, then compare (bincmp.py). `fakcu` beside a gap is a false fit, a stray `ret`. WEGA=dir: the original tree.
"""
import glob
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recon'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402

WEGA = os.environ.get('WEGA', '/home/olivleh1/git/P8000/tmp/wega31_root')
HERE = os.path.dirname(os.path.abspath(__file__))
EMU = os.path.normpath(os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu'))


def norm(t):
    return re.sub(r'%[0-9A-Fa-f]{1,8}\b', 'A', t)


def load(path):
    o = SOut(open(path, 'rb').read())
    c = o.segs[0]['code']
    return bytes(o.image)[:c], c


def members(lib):
    work = tempfile.mkdtemp()
    subprocess.run([EMU, '-r', WEGA, '/bin/ar', 'x', os.path.relpath(lib, work)], cwd=work, capture_output=True)
    out = []
    for f in sorted(os.listdir(work)):
        if f.endswith('.o'):
            try:
                code, c = load(os.path.join(work, f))
                if c:
                    out.append((f[:-2], code, [norm(t) for _, t in zcode.insns(code, 0, c, False)]))
            except Exception:
                pass
    return out


def walk(gc, A, NA, addr, M, i):
    rows, gap, cov = [], None, 0
    while i < len(A):
        best = None
        for n, code, N in M:
            m = len(N)
            if m and NA[i:i + m] == N:
                a = addr[i]
                key = (len(code), sum(x == y for x, y in zip(gc[a:a + len(code)], code)))
                if best is None or key > best[0]:
                    best = (key, n, m)
        if best is None:
            gap = addr[i] if gap is None else gap
            i += 1
            continue
        if gap is not None:
            rows.append('# gap %x .. %x, %d bytes' % (gap, addr[i], addr[i] - gap))
            gap = None
        rows.append('%s\t# %x, %d bytes' % (best[1], addr[i], best[0][0]))
        cov += best[0][0]
        i += best[2]
    if gap is not None:
        rows.append('# gap %x .. end' % gap)
    return cov, rows


def main():
    prog = sys.argv[1]
    pos = int(sys.argv[2], 16)
    d = sys.argv[3] if len(sys.argv) > 3 else os.path.normpath(os.path.join(HERE, '..', '..', 'src', 'lib'))
    gc, cg = load(prog)
    A = list(zcode.insns(gc, 0, cg, False))
    addr = [a for a, _ in A]
    NA = [norm(t) for _, t in A]
    i = next(k for k, a in enumerate(addr) if a >= pos)
    res = []
    for lib in sorted(glob.glob(d + '/libc-v*.a')) + [d + '/libc/libc.a']:
        cov, rows = walk(gc, A, NA, addr, members(lib), i)
        res.append((cov, sum(r.startswith('#') for r in rows), lib, rows))
    res.sort(key=lambda r: (r[0], -r[1]))
    for cov, gaps, lib, _ in res:
        print('%-14s %6d of %d bytes, %d gaps' % (os.path.basename(lib), cov, cg - addr[i], gaps))
    print('\n'.join(res[-1][3]))


if __name__ == '__main__':
    main()
