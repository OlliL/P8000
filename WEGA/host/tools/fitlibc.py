#!/usr/bin/env python3
"""fitlibc.py PROGRAM OWNEND [OWNLIST]: the libc part of a linked WEGA program as a link order of known modules.

Walks through the text of PROGRAM from OWNEND (hex: the end of the own code) and takes at each place the module whose
code fits: the members of the shipped /lib/libc.a and the versions in src/lib/oldlibc. Addresses are masked for the fit;
among the modules that fit, the one with the most equal bytes wins, so that constants decide between versions (length
0_2 and 0_3). A place where nothing fits is printed as a gap with its size; such a module has to be cut (unlink.py).

Prints one line per module: `old NAME 0_n` or `shipped NAME`, the form of a .link file. WEGA=dir: the original tree.
"""
import difflib
import os
import re
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'recon'))
import zcode  # noqa: E402
from sout import SOut  # noqa: E402

WEGA = os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root')
HERE = os.path.dirname(os.path.abspath(__file__))
OLD = os.path.normpath(os.path.join(HERE, '..', '..', 'src', 'lib', 'oldlibc'))
EMU = os.path.normpath(os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu'))


def norm(t):
    return re.sub(r'%[0-9A-Fa-f]{1,8}\b', 'A', t)


def load(path):
    o = SOut(open(path, 'rb').read())
    c = o.segs[0]['code']
    return bytes(o.image)[:c], c


def modules():
    """(kind, name, version, code, normalised instructions)"""
    res = []
    work = tempfile.mkdtemp()
    subprocess.run([EMU, '-r', WEGA, '/bin/ar', 'x', '/lib/libc.a'], cwd=work, capture_output=True)
    for f in sorted(os.listdir(work)):
        if f.endswith('.o'):
            res.append(('shipped', f[:-2], '', os.path.join(work, f)))
    for d in sorted(os.listdir(OLD)):
        p = os.path.join(OLD, d)
        if not os.path.isdir(p):
            continue
        for f in sorted(os.listdir(p)):
            if not f.endswith('.s'):
                continue
            o = os.path.join(p, f[:-2] + '.o')
            if not os.path.exists(o):
                t = os.path.join(work, d + f[:-2] + '.o')
                subprocess.run([EMU, '-r', WEGA, '/bin/cas', '-u', '-o', os.path.relpath(t), os.path.relpath(os.path.join(p, f))],
                               capture_output=True)
                o = t
            if os.path.exists(o):
                res.append(('old', d, f[:-2], o))
    out = []
    for k, n, v, o in res:
        try:
            code, c = load(o)
            if c:
                out.append((k, n, v, code, [norm(t) for _, t in zcode.insns(code, 0, c, False)]))
        except Exception:
            pass
    return out


def gap(lo, hi, A, NA, addr, M):
    """a place where no module fits: the most similar modules, as a hint"""
    i = addr.index(lo)
    j = addr.index(hi) if hi in addr else len(addr)
    seq = NA[i:j]
    best = sorted(((difflib.SequenceMatcher(None, seq, N, autojunk=False).quick_ratio(), k, n, v) for k, n, v, _, N in M if len(N) > 2),
                  reverse=True)[:3]
    print('# gap %x .. %x, %d bytes, like: %s' % (lo, hi, hi - lo, ', '.join('%s %s %.2f' % (n, v, r) for r, k, n, v in best)))


def main():
    prog = sys.argv[1]
    pos = int(sys.argv[2], 16)
    gc, cg = load(prog)
    A = list(zcode.insns(gc, 0, cg, False))
    addr = [a for a, _ in A]
    NA = [norm(t) for _, t in A]
    M = modules()
    i = next(k for k, a in enumerate(addr) if a >= pos)
    gapstart = None
    while i < len(A):
        best = None
        for k, n, v, code, N in M:
            m = len(N)
            if m and NA[i:i + m] == N:
                a = addr[i]
                eq = sum(1 for x, y in zip(gc[a:a + len(code)], code) if x == y)
                key = (len(code), eq, k == 'shipped')
                if best is None or key > best[0]:
                    best = (key, k, n, v, m, len(code))
        if best is None:
            if gapstart is None:
                gapstart = addr[i]
            i += 1
            continue
        if gapstart is not None:
            gap(gapstart, addr[i], A, NA, addr, M)
            gapstart = None
        _, k, n, v, m, size = best
        print('%s %s%s\t# %x, %d bytes' % ('old' if k == 'old' else 'shipped', n, ' ' + v if v else '', addr[i], size))
        i += m
    if gapstart is not None:
        gap(gapstart, cg, A, NA, addr, M)


if __name__ == '__main__':
    main()
