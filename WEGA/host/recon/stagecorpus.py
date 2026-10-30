#!/usr/bin/env python3
"""stagecorpus.py - corpus test of one pass of the compiler: the original and our binary on the same inputs.

Usage: stagecorpus.py OURS [-k] [-n N] [-f FILTER] [SRC ...]

  OURS      our binary of the pass (a z8k program, run under z8k-emu)
  SRC       C sources (default: progcfg CORPUS_SOURCES(), e.g. every .c of src/cmd and src/lib)
  -n N      only the first N sources;  -f FILTER  only sources whose path contains FILTER
  -k        keep the work directories of the differing cases (below WORK)

The pass is described by progcfg (functions of the program's progcfg.py):
  CORPUS_PREPARE(src, work)        writes the input of the pass into work (with the original earlier passes), returns
                                   the base name, or None when the source cannot be prepared (the case is skipped)
  CORPUS_RUN(binary, base, work)   (argv, stdin file or None) of the pass on that input (binary: guest path of the pass)
  CORPUS_OUTPUTS(base)             the files the pass writes (compared byte for byte)
The exit status and the messages (stdout, stderr) are compared too. Result: one line per differing case and the totals.
"""
import os
import shutil
import subprocess
import sys

import progcfg

CFG = progcfg.load()
WORK = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'corpus_' + CFG.NAME)


def run_pass(binary, base, work):
    argv, stdin = CFG.CORPUS_RUN(binary, base, work)
    fin = open(os.path.join(work, stdin), 'rb') if stdin else subprocess.DEVNULL
    r = subprocess.run([CFG.EMU, '-r', CFG.ROOT] + argv, cwd=work, stdin=fin, capture_output=True, timeout=600)
    outs = {}
    for f in CFG.CORPUS_OUTPUTS(base):
        p = os.path.join(work, f)
        outs[f] = open(p, 'rb').read() if os.path.exists(p) else None
        if os.path.exists(p):
            os.remove(p)
    return r.returncode, r.stdout + r.stderr, outs


def main():
    a = sys.argv[1:]
    keep = '-k' in a
    a = [x for x in a if x != '-k']
    n = flt = None
    if '-n' in a:
        i = a.index('-n'); n = int(a[i + 1]); del a[i:i + 2]
    if '-f' in a:
        i = a.index('-f'); flt = a[i + 1]; del a[i:i + 2]
    ours = os.path.abspath(a[0])
    srcs = a[1:] or CFG.CORPUS_SOURCES()
    if flt:
        srcs = [s for s in srcs if flt in s]
    if n:
        srcs = srcs[:n]
    shutil.rmtree(WORK, ignore_errors=True)
    os.makedirs(WORK)
    total = same = skipped = 0
    for k, src in enumerate(srcs):
        work = os.path.join(WORK, '%04d' % k)
        os.makedirs(work)
        base = CFG.CORPUS_PREPARE(src, work)
        if base is None:
            skipped += 1
            shutil.rmtree(work)
            continue
        total += 1
        shutil.copy(ours, os.path.join(work, 'pass_new'))
        r1 = run_pass('/' + CFG.ORIG_BIN, base, work)
        r2 = run_pass('pass_new', base, work)
        diffs = []
        if r1[0] != r2[0]:
            diffs.append('rc %d/%d' % (r1[0], r2[0]))
        if r1[1] != r2[1]:
            diffs.append('messages')
        for f in r1[2]:
            if r1[2][f] != r2[2][f]:
                diffs.append('%s %s/%s' % (f, None if r1[2][f] is None else len(r1[2][f]), None if r2[2][f] is None else len(r2[2][f])))
        if diffs:
            print('DIFF %s: %s' % (src, ', '.join(diffs)), flush=True)
            if not keep:
                shutil.rmtree(work)
        else:
            same += 1
            shutil.rmtree(work)
    print('%s: %d of %d inputs identical (%d sources skipped)' % (CFG.NAME, same, total, skipped))


if __name__ == '__main__':
    main()
