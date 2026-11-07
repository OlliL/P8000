#!/usr/bin/env python3
"""stagecorpus.py - corpus test of a program: the original and our binary on the same inputs.

Usage: RECON_PROG=NAME stagecorpus.py [-k] [-n N] [-f FILTER]     (normally through host/tools/corpus.sh NAME)

  -n N      only the first N inputs;  -f FILTER  only inputs whose name contains FILTER
  -k        keep the work directories of the differing cases (below WORK)

The program is described by the functions and values of its progcfg.py (src/cmd/NAME/tools):
  CORPUS_BINARY                    our binary, relative to SRCDIR (environment CORPUS_BIN: another file instead)
  CORPUS_SOURCES()                 the inputs (e.g. every .c of src/cmd and src/lib, or command lines)
  CORPUS_PREPARE(src, work)        writes the input into work (made with the original earlier passes), returns the base
                                   name, or None when the source cannot be prepared (the case is skipped)
  CORPUS_RUN(binary, base, work[, variant])  (argv, stdin file or None) of one run (binary: guest path)
  CORPUS_OUTPUTS(base)             the files a run writes (compared byte for byte)
  CORPUS_VARIANTS                  optional: a list of variants (e.g. options), each input is run once per variant
  CORPUS_SCRIPT(base)              optional, script mode: the shell script of the case (path), for tools that are driven by many
                                   commands (ar) instead of one run: it is run by sh in a directory of its own with
                                   PROG = the program under test as a command line, CASE = base, and the environment of the
                                   variant (a dict, e.g. {'TZ': 'MET-1MEST'}; the guest gets it by -e); compared are its
                                   output, its exit status and the files that are there afterwards (mode, size, checksum).
                                   CORPUS_PREPARE puts the files the case starts with into work; CORPUS_RUN is not used.
  CORPUS_IGNORE                    optional: a regular expression (bytes) of message text that may differ
  CORPUS_NORMALIZE(file, data)     optional: an output file as it is compared (bytes that are known to be undefined)
JOBS inputs at a time (default: all cores but one), below STAGE_TMP (default /tmp/wega_stage, a local disk).
The exit status and the messages (stdout, stderr) are compared too. Result: one line per differing case and the totals;
exit status 1 when a case differs.
"""
import concurrent.futures
import os
import re
import shlex
import shutil
import subprocess
import sys
import zlib

import progcfg

CFG = progcfg.load()
WORK = os.path.join(os.environ.get('STAGE_TMP', '/tmp/wega_stage'), 'corpus_' + CFG.NAME)


VARIANTS = getattr(CFG, 'CORPUS_VARIANTS', [None])
IGNORE = re.compile(CFG.CORPUS_IGNORE) if getattr(CFG, 'CORPUS_IGNORE', None) else None
NORMALIZE = getattr(CFG, 'CORPUS_NORMALIZE', None)
HOST = bool(os.environ.get('CORPUS_HOST'))	# CORPUS_BIN is a host (Linux) program, not a WEGA binary


def run_script(binary, base, work, variant, tag):
    """script mode: the script of the case with PROG = the program; the result as of run_pass"""
    rd = os.path.join(work, 'run_' + tag)
    os.makedirs(rd)
    for f in os.listdir(work):
        p = os.path.join(work, f)
        if os.path.isfile(p) and f != 'pass_new':
            shutil.copy(p, rd)
    if binary == 'pass_new':
        # our program under its own name (the messages show argv[0]): <run>/.prog/NAME
        os.makedirs(os.path.join(rd, '.prog'))
        shutil.copy(os.path.join(work, 'pass_new'), os.path.join(rd, '.prog', CFG.NAME))
        binary = '.prog/' + CFG.NAME
        ours = True
    else:
        ours = False
    env = dict(os.environ)
    env.pop('TZ', None)
    env['LC_ALL'] = 'C'
    var = variant or {}
    env.update(var)
    env['CASE'] = base
    if HOST and ours:
        prog = os.path.join(rd, binary)
    else:
        prog = ' '.join(shlex.quote(x) for x in [CFG.EMU] + sum((['-e', '%s=%s' % kv] for kv in var.items()), [])
                        + ['-r', CFG.ROOT, binary])
    env['PROG'] = prog
    script = os.path.join(CFG.DIR, CFG.CORPUS_SCRIPT(base))
    r = subprocess.run(['sh', script], cwd=rd, capture_output=True, timeout=1800, env=env)
    lines = []
    for f in sorted(os.listdir(rd)):
        p = os.path.join(rd, f)
        if os.path.isfile(p):
            d = open(p, 'rb').read()
            lines.append('%o %d %08x %s' % (os.stat(p).st_mode & 0o7777, len(d), zlib.crc32(d), f))
    shutil.rmtree(rd, ignore_errors=True)
    msg = r.stdout + r.stderr
    if NORMALIZE:
        msg = NORMALIZE('messages', msg)
    return r.returncode, msg, {'files': '\n'.join(lines).encode()}


def run_pass(binary, base, work, variant, tag=''):
    if hasattr(CFG, 'CORPUS_SCRIPT'):
        return run_script(binary, base, work, variant, tag)
    argv, stdin = CFG.CORPUS_RUN(binary, base, work) if variant is None else CFG.CORPUS_RUN(binary, base, work, variant)
    fin = open(os.path.join(work, stdin), 'rb') if stdin else subprocess.DEVNULL
    if HOST and binary == 'pass_new':
        # a host build (CORPUS_HOST=1): runs natively; the environment from CORPUS_HOST_ENV(root) of progcfg.py
        env = dict(os.environ)
        env.update(getattr(CFG, 'CORPUS_HOST_ENV', lambda root: {})(CFG.ROOT))
        r = subprocess.run([os.path.join(work, 'pass_new')] + argv[1:], cwd=work, stdin=fin, capture_output=True,
                           timeout=600, env=env)
    else:
        r = subprocess.run([CFG.EMU, '-r', CFG.ROOT] + argv, cwd=work, stdin=fin, capture_output=True, timeout=600)
    outs = {}
    for f in CFG.CORPUS_OUTPUTS(base):
        p = os.path.join(work, f)
        outs[f] = open(p, 'rb').read() if os.path.exists(p) else None
        if outs[f] is not None and NORMALIZE:
            outs[f] = NORMALIZE(f, outs[f])
        if os.path.exists(p):
            os.remove(p)
    msg = r.stdout + r.stderr
    if NORMALIZE:
        msg = NORMALIZE('messages', msg)
    if IGNORE:
        msg = IGNORE.sub(b'', msg)
    return r.returncode, msg, outs


def main():
    a = sys.argv[1:]
    keep = '-k' in a
    a = [x for x in a if x != '-k']
    n = flt = None
    if '-n' in a:
        i = a.index('-n'); n = int(a[i + 1]); del a[i:i + 2]
    if '-f' in a:
        i = a.index('-f'); flt = a[i + 1]; del a[i:i + 2]
    ours = os.environ.get('CORPUS_BIN') or os.path.join(CFG.SRCDIR, CFG.CORPUS_BINARY)
    if not os.path.exists(ours):
        sys.exit('%s: %s not built' % (CFG.NAME, ours))
    srcs = CFG.CORPUS_SOURCES()
    if flt:
        srcs = [s for s in srcs if flt in s]
    if n:
        srcs = srcs[:n]
    shutil.rmtree(WORK, ignore_errors=True)
    os.makedirs(WORK)

    def case(k, src):
        """(skipped, differences) of one source, in a directory of its own"""
        work = os.path.join(WORK, '%04d' % k)
        os.makedirs(work)
        base = CFG.CORPUS_PREPARE(src, work)
        if base is None:
            shutil.rmtree(work)
            return True, []
        diffs = []
        for v in VARIANTS:
            shutil.copy(ours, os.path.join(work, 'pass_new'))
            r1 = run_pass('/' + CFG.ORIG_BIN, base, work, v, 'orig')
            r2 = run_pass('pass_new', base, work, v, 'new')
            tag = '' if v is None else '[%s] ' % (' '.join('%s=%s' % kv for kv in v.items()) if isinstance(v, dict) else ' '.join(v))
            if r1[0] != r2[0]:
                diffs.append('%src %d/%d' % (tag, r1[0], r2[0]))
            if r1[1] != r2[1]:
                diffs.append(tag + 'messages')
            for f in r1[2]:
                if r1[2][f] != r2[2][f]:
                    diffs.append('%s%s %s/%s' % (tag, f, None if r1[2][f] is None else len(r1[2][f]),
                                                 None if r2[2][f] is None else len(r2[2][f])))
        if not (diffs and keep):
            shutil.rmtree(work)
        return False, diffs

    total = same = skipped = 0
    jobs = int(os.environ.get('JOBS') or max(1, (os.cpu_count() or 2) - 1))
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        futs = [ex.submit(case, k, src) for k, src in enumerate(srcs)]
        for src, f in zip(srcs, futs):
            skip, diffs = f.result()
            if skip:
                skipped += 1
                continue
            total += 1
            if diffs:
                print('DIFF %s: %s' % (src, ', '.join(diffs)), flush=True)
            else:
                same += 1
    print('%s: %d of %d inputs identical (%d sources skipped)' % (CFG.NAME, same, total, skipped))
    return 1 if same != total else 0


if __name__ == '__main__':
    sys.exit(main())
