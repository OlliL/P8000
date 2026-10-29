#!/usr/bin/env python3
"""corpus_ld.py - Schritt 6: the corpus test of the rebuilt ld against the original /bin/ld.

usage: corpus_ld.py LDBIN [-n RANDOM_RUNS] [-s SEED]
  LDBIN  the rebuilt ld (a z8k program, run under z8k-emu from the work directory)
  environment: WEGA = file tree of the original system (bin/ld, lib/libc.a, usr/bin/ranlib), EMU = emulator

Uses the corpus of host/ld/check.py (fixed command lines and random ones, the same test objects and libraries):
the output file and the exit status must be identical. Our ld is run as `z8k-emu -r WEGA ld_under_test ARGS` in the
work directory, the original as `z8k-emu -r WEGA /bin/ld ARGS`.
"""
import importlib.util
import os
import shutil
import subprocess
import sys

HOSTLD = '/mnt/share/cvs/P8000/WEGA/host/ld/check.py'
spec = importlib.util.spec_from_file_location('check', HOSTLD)
C = importlib.util.module_from_spec(spec)
spec.loader.exec_module(C)
C.ROOT = os.path.abspath(os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root'))
C.EMU = os.environ.get('EMU', '/mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu')
LDBIN = os.path.abspath(sys.argv[1])


def link1(args, tool, base):
    res = []
    for who in ('orig', 'ours'):
        d = os.path.join(C.W, who)
        shutil.rmtree(d, ignore_errors=True)
        shutil.copytree(os.path.join(C.W, base), d, copy_function=shutil.copy2)
        if who == 'orig':
            r = C.orig(args, d, tool)
        else:
            shutil.copy(LDBIN, os.path.join(d, 'ld_under_test'))
            r = subprocess.run([C.EMU, '-r', C.ROOT, 'ld_under_test'] + args, cwd=d, capture_output=True, timeout=300)
        out = 'a.out'
        for i, a in enumerate(args):
            if a == '-o' and i + 1 < len(args):
                out = args[i + 1]
        p = os.path.join(d, out)
        res.append((r.returncode, open(p, 'rb').read() if os.path.exists(p) else None, r.stderr.decode(errors='replace')))
    (rc1, o1, e1), (rc2, o2, e2) = res
    if rc1 == rc2 and o1 == o2:
        return True, ''
    return False, '%s: rc %d/%d, size %s/%s\n  orig: %s\n  ours: %s' % (
        ' '.join(args), rc1, rc2, len(o1) if o1 is not None else None, len(o2) if o2 is not None else None,
        e1.replace('\n', '|')[:200], e2.replace('\n', '|')[:200])


C.link1 = link1
C.link = lambda args, tool='ld', base='base': link1(args, tool, base)


def main():
    n, seed = 100, 1
    a = sys.argv[2:]
    while a:
        o = a.pop(0)
        if o == '-n': n = int(a.pop(0))
        elif o == '-s': seed = int(a.pop(0))
    C.W = os.path.join('/mnt/share/cvs/P8000/tmp/tmp', 'corpus_ld_work')
    shutil.rmtree(C.W, ignore_errors=True)
    os.makedirs(C.W)
    C.setup()
    bad = total = 0
    for line in C.FIXED:
        total += 1
        ok, msg = C.link(line.split())
        if not ok:
            bad += 1
            print('DIFF', msg, flush=True)
    for args in C.rnd(n, seed):
        total += 1
        ok, msg = C.link(args)
        if not ok:
            bad += 1
            print('DIFF', msg, flush=True)
    print('ld: %s (%d of %d command lines identical)' % ('identical' if not bad else 'DIFFERENT', total - bad, total), flush=True)


if __name__ == '__main__':
    main()
