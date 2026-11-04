#!/usr/bin/env python3
"""libcfit.py [-v] [-c COMPILER] SOURCE.c MODULE...: try one C source against modules of oldlibc or oldcc, with every compiler.

MODULE is NAME/VERSION of src/lib/oldlibc, or oldcc:NAME/VERSION of src/lib/libc/oldcc, or shipped:NAME. Prints per module the
best compiler and headers: instructions that differ (addresses masked), then bytes of code and data that differ; EXACT
when all are 0. -v shows the differing blocks of the best one. Other headers of the directory of SOURCE are used.
"""
import difflib
import glob
import os
import shutil
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import libctable as L  # noqa: E402
import zcode  # noqa: E402


def compile_all(src):
    res = []
    has = 'stdio.h' in open(src, errors='replace').read()
    for name, root, cc in L.COMPILERS:
        for h in (('guest', 'cparse') if has else ('guest',)):
            w = tempfile.mkdtemp(dir=L.TMP)
            for f in glob.glob(os.path.dirname(os.path.abspath(src)) + '/*.h'):
                shutil.copy(f, w)
            shutil.copy(src, w)
            if h == 'cparse':
                shutil.copy(os.path.join(L.REPO, 'src/cmd/cparse/stdio.h'), w)
            b = os.path.basename(src)
            r = subprocess.run([L.EMU, '-r', root, cc, '-O', '-I.', '-c', b], cwd=w, capture_output=True, timeout=120)
            p = os.path.join(w, b[:-2] + '.o')
            if os.path.exists(p):
                res.append((name, h) + L.parts(p) + (L.names(p),))
            else:
                print('no object', name, h, (r.stdout + r.stderr)[:200])
            shutil.rmtree(w, ignore_errors=True)
    return res


def module(m):
    kind = 'old'
    if ':' in m:
        kind, m = m.split(':')
    if kind == 'shipped':
        d = os.path.join(L.TMP, 'orig', m + '.o')
    else:
        d = os.path.join(L.TMP, 'mods', kind + '_' + m.replace('/', '_') + '.o')
    return L.parts(d) + (L.names(d),)


def show(c):
    return [t.replace('\t', ' ') for _, t in zcode.insns(c, 0, len(c), False)]


def main():
    a = sys.argv[1:]
    v = '-v' in a
    a = [x for x in a if x != '-v']
    if '-c' in a:
        i = a.index('-c')
        L.COMPILERS = [c for c in L.COMPILERS if c[0] == a[i + 1]]
        del a[i:i + 2]
    L.targets()
    objs = compile_all(a[0])
    for m in a[1:]:
        c, d, nm = module(m)
        A = show(c)
        na = [L.norm(x) for x in A]
        best = None
        for name, h, c2, d2, nm2 in objs:
            B = show(c2)
            nb = [L.norm(x) for x in B]
            ops = [o for o in difflib.SequenceMatcher(None, na, nb, autojunk=False).get_opcodes() if o[0] != 'equal']
            n = sum(max(o[2] - o[1], o[4] - o[3]) for o in ops)
            bc = sum(x != y for x, y in zip(c, c2)) + abs(len(c) - len(c2))
            bd = sum(x != y for x, y in zip(d, d2)) + abs(len(d) - len(d2))
            k = (n, bc, bd, len(nm ^ nm2))
            if best is None or k < best[0]:
                best = (k, name, h, ops, A, B, sorted(nm ^ nm2))
        k, name, h, ops, A, B, nd = best
        print('%-16s %s %s %s: insns %d, bytes code %d data %d' % (m, 'EXACT' if k == (0, 0, 0, 0) else 'differs', name, h, k[0], k[1], k[2]) + (', names %s' % nd if nd else ''))
        if v:
            if not ops:
                for x, y in zip(A, B):
                    if x != y:
                        print('   mod', x, ' src', y)
            for o, i, j, kk, ll in ops:
                print('   mod', A[max(0, i - 1):j + 1][:10])
                print('   src', B[max(0, kk - 1):ll + 1][:10])


if __name__ == '__main__':
    main()
