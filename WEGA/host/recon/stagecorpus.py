#!/usr/bin/env python3
"""stagecorpus.py - corpus test of a program: the original and our binary on the same inputs.

Usage: RECON_PROG=NAME stagecorpus.py [-c CANDIDATE[,CANDIDATE]] [-k] [-n N] [-f FILTER]     (normally through host/tools/corpus.sh NAME)

  -s        with -c: a program without such a candidate is not an error
  -c        only these candidates (wega, host); default: every candidate of progcfg CANDIDATES

  -n N      only the first N inputs;  -f FILTER  only inputs whose name contains FILTER
  -k        keep the work directories of the differing cases (below WORK)

The program is described by the functions and values of its progcfg.py (compare/NAME):
  CANDIDATES                       what is tested against the original: a dict, 'wega' (the WEGA program built from src, runs under
                                   z8k-emu) and/or 'host' (the host build, runs natively), each the path of the binary relative to the
                                   WEGA directory; the original runs once per input and every candidate is compared with it.
                                   Default: {'wega': SRCDIR/CORPUS_BINARY}. Environment CORPUS_BIN_WEGA, CORPUS_BIN_HOST: another file.
  CORPUS_SOURCES()                 the inputs (e.g. every .c of src/cmd and src/lib, or command lines)
  CORPUS_PREPARE(src, work)        writes the input into work (made with the original earlier passes), returns the base
                                   name, or None when the source cannot be prepared (the case is skipped)
  CORPUS_RUN(binary, base, work[, variant])  (argv, stdin file or None) of one run (binary: guest path, or pass_NAME of a candidate; the framework runs it as .prog_NAME/PROGRAM)
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
The inputs are prepared once and kept in a tar (compare/NAME/corpus.tar, not in git; CORPUS_OF in progcfg.py names another program's
directory: casold uses the tar of cas): normal runs only unpack it; a missing tar is made (under a lock) first, `--make` makes it new
(host/tools/mkcorpus.sh does that and removes the cache). Results are cached beside it (compare/NAME/cache/, not in git):
  - the original's output per case, key = sha1 of tar, of the program code of the corpus (*.py, *.sh of the directory and of host/recon),
    of the emulator, of the original binary and of the files of ROOT the runs read (progcfg CORPUS_ROOT_FILES: headers, libraries, a program
    the original runs);
  - "candidate identical" per candidate, key = that key + sha1 of the candidate binary + progcfg CORPUS_DEPENDS[candidate] (files or
    directories the candidate runs, relative to the WEGA directory). A hit means the same binary was already tested against the same
    original on the same inputs: nothing runs. Failures are never cached. CORPUS_NOCACHE=1 ignores the cache (it is still written);
    `--keys` prints the parts of the keys. CORPUS_STORE: directory that holds the compare/NAME directories (default: beside the progcfg).
JOBS inputs at a time (default: all cores but one), below STAGE_TMP (default /tmp/wega_stage, a local disk).
The exit status and the messages (stdout, stderr) are compared too. Result: one line per differing case and the totals;
exit status 1 when a case differs.
"""
import concurrent.futures
import fcntl
import hashlib
import io
import pickle
import tarfile
import threading
import time
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

_STRACE_N = [0]


def strace_prefix(tag):
    """STAGE_STRACE=DIR: every run of the original or of a candidate under strace (the files it opens, the programs it starts), one
    file per run in DIR (named TAG.N); host/tools/corpusdeps.py reads them: the dependencies that belong in CORPUS_DEPENDS"""
    d = os.environ.get('STAGE_STRACE')
    if not d:
        return []
    os.makedirs(d, exist_ok=True)
    with PHASE_LOCK:
        _STRACE_N[0] += 1
        n = _STRACE_N[0]
    return ['strace', '-f', '-qq', '-e', 'trace=openat,open,execve,access,stat,newfstatat', '-o', os.path.join(d, '%s.%s.%d' % (CFG.NAME, tag, n))]


def run_script(binary, base, work, variant, tag, native):
    """script mode: the script of the case with PROG = the program; the result as of run_pass"""
    rd = os.path.join(work, 'run_' + tag)
    os.makedirs(rd)
    for f in os.listdir(work):
        p = os.path.join(work, f)
        if f.startswith(('pass_', '.prog_', 'run_')):
            continue
        if os.path.isdir(p):
            shutil.copytree(p, os.path.join(rd, f))
        elif os.path.isfile(p):
            shutil.copy(p, rd)
    if binary.startswith('pass_'):
        # a candidate under its own name (the messages show argv[0]): <run>/.prog/NAME
        os.makedirs(os.path.join(rd, '.prog'))
        shutil.copy(os.path.join(work, binary), os.path.join(rd, '.prog', CFG.NAME))
        binary = '.prog/' + CFG.NAME
    env = dict(os.environ)
    env.pop('TZ', None)
    env['LC_ALL'] = 'C'
    var = variant or {}
    env.update(var)
    env['CASE'] = base
    if native:
        env.update(getattr(CFG, 'CORPUS_HOST_ENV', lambda root: {})(CFG.ROOT))
    if native:
        prog = ' '.join(shlex.quote(x) for x in strace_prefix(tag) + [os.path.join(rd, binary)])
    else:
        prog = ' '.join(shlex.quote(x) for x in strace_prefix(tag) + [CFG.EMU] + sum((['-e', '%s=%s' % kv] for kv in var.items()), [])
                        + ['-r', CFG.ROOT, binary])
    env['PROG'] = prog
    script = os.path.join(CFG.DIR, CFG.CORPUS_SCRIPT(base))
    r = subprocess.run(['sh', script], cwd=rd, capture_output=True, timeout=1800, env=env)
    lines = []
    for dp, dn, fn in os.walk(rd):
        dn[:] = [x for x in dn if not (dp == rd and x == '.prog')]
        for f in fn:
            p = os.path.join(dp, f)
            d = open(p, 'rb').read()
            lines.append('%o %d %08x %s' % (os.stat(p).st_mode & 0o7777, len(d), zlib.crc32(d), os.path.relpath(p, rd)))
    lines.sort(key=lambda l: l.split(' ', 3)[3])
    shutil.rmtree(rd, ignore_errors=True)
    msg = r.stdout + r.stderr
    if NORMALIZE:
        msg = NORMALIZE('messages', msg)
    return r.returncode, msg, {'files': '\n'.join(lines).encode()}


# time of the phases (CPU seconds of the threads added up): prepare, original, each candidate, the rest of a case; printed at the end as TIME
PHASES = {}
PHASE_LOCK = threading.Lock()


def phase_add(name, seconds):
    with PHASE_LOCK:
        PHASES[name] = PHASES.get(name, 0.0) + seconds


def run_pass(binary, base, work, variant, tag='', native=False):
    """one run of the original (binary: guest path), or of a candidate (binary: pass_NAME in work; native: a host program):
    (exit status, messages, {output file: content})"""
    if hasattr(CFG, 'CORPUS_SCRIPT'):
        return run_script(binary, base, work, variant, tag, native)
    argv, stdin = CFG.CORPUS_RUN(binary, base, work) if variant is None else CFG.CORPUS_RUN(binary, base, work, variant)
    if binary.startswith('pass_') and argv[0] == binary:
        argv = ['.prog_%s/%s' % (binary[5:], CFG.NAME)] + argv[1:]	# a candidate under the name of the program (argv[0] is in messages)
    fin = open(os.path.join(work, stdin), 'rb') if stdin else subprocess.DEVNULL
    if native:
        # a host build: runs natively; the environment from CORPUS_HOST_ENV(root) of progcfg.py
        env = dict(os.environ)
        env.update(getattr(CFG, 'CORPUS_HOST_ENV', lambda root: {})(CFG.ROOT))
        r = subprocess.run(strace_prefix(tag) + [os.path.join(work, argv[0])] + argv[1:], cwd=work, stdin=fin, capture_output=True,
                           timeout=600, env=env)
    else:
        r = subprocess.run(strace_prefix(tag) + [CFG.EMU, '-r', CFG.ROOT] + argv, cwd=work, stdin=fin, capture_output=True, timeout=600)
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


# ---- the tar of the prepared inputs and the cache of the results ------------------------------------------------------------------------

STORE = os.environ.get('CORPUS_STORE') or os.path.dirname(CFG.DIR)
HOME = os.path.join(STORE, getattr(CFG, 'CORPUS_OF', os.path.basename(CFG.DIR)))
TAR = os.path.join(HOME, 'corpus.tar')
CACHE = os.path.join(HOME, 'cache')
_SHA = {}


def sha1file(path):
    if path not in _SHA:
        h = hashlib.sha1()
        with open(path, 'rb') as f:
            for blk in iter(lambda: f.read(1 << 20), b''):
                h.update(blk)
        _SHA[path] = h.hexdigest()
    return _SHA[path]


def sha1tree(path, skip=()):
    """sha1 of the names and contents of a file or directory (symbolic links by their target, no devices)"""
    h = hashlib.sha1()
    if os.path.isfile(path):
        return sha1file(path)
    for dp, dn, fn in os.walk(path):
        dn[:] = sorted(x for x in dn if os.path.relpath(os.path.join(dp, x), path) not in skip)
        for f in sorted(fn):
            p = os.path.join(dp, f)
            h.update(os.path.relpath(p, path).encode() + b'\0')
            if os.path.islink(p):
                h.update(b'L' + os.readlink(p).encode())
            elif os.path.isfile(p):
                h.update(sha1file(p).encode())
    return h.hexdigest()


def code_key():
    """the program code of the corpus: *.py and *.sh of the directory of the program, of the one of the tar, and of host/recon"""
    h = hashlib.sha1()
    for lab, d in (('prog', CFG.DIR), ('home', HOME), ('recon', os.path.join(CFG.REPO, 'host', 'recon'))):
        for dp, dn, fn in os.walk(d):
            dn[:] = sorted(x for x in dn if x not in ('cache', '__pycache__'))
            for f in sorted(fn):
                if f.endswith(('.py', '.sh')):
                    p = os.path.join(dp, f)
                    h.update(lab.encode() + os.path.relpath(p, d).encode() + b'\0' + sha1file(p).encode())
    return h.hexdigest()


def key_parts(cands):
    """{part: sha1}: what the cache keys are made of (--keys prints them)"""
    k = {'tar': sha1file(TAR), 'code': code_key(), 'emulator': sha1file(CFG.EMU),
         'original': sha1file(os.path.join(CFG.ROOT, CFG.ORIG_BIN)), 'variants': hashlib.sha1(repr(VARIANTS).encode()).hexdigest(),
         }
    for dep in getattr(CFG, 'CORPUS_ROOT_FILES', []):
        q = os.path.join(CFG.ROOT, dep)
        k['root ' + dep] = sha1tree(q) if os.path.exists(q) else 'missing'
    for name, path in cands.items():
        k['binary ' + name] = sha1file(path)
        for dep in getattr(CFG, 'CORPUS_DEPENDS', {}).get(name, []):
            k['depends %s %s' % (name, dep)] = sha1tree(os.path.join(CFG.REPO, dep))
    return k


def mix(parts):
    return hashlib.sha1('\n'.join('%s %s' % kv for kv in sorted(parts.items())).encode()).hexdigest()[:20]


def make_tar(excl=None):
    """prepare every input and write HOME/corpus.tar (deterministic: sorted, no times, no owners); under a lock"""
    os.makedirs(HOME, exist_ok=True)
    with open(os.path.join(HOME, '.lock'), 'w') as lk:
        fcntl.flock(lk, fcntl.LOCK_EX)
        if os.path.exists(TAR) and not excl:
            return
        t0 = time.monotonic()
        srcs = CFG.CORPUS_SOURCES()
        wk = os.path.join(os.path.dirname(WORK), 'make_' + os.path.basename(WORK))
        shutil.rmtree(wk, ignore_errors=True)
        os.makedirs(wk)

        def prep(k, src):
            work = os.path.join(wk, '%04d' % k)
            os.makedirs(work)
            return CFG.CORPUS_PREPARE(src, work)

        def norm(ti):
            ti.mtime = 0
            ti.uid = ti.gid = 0
            ti.uname = ti.gname = ''
            return ti

        tmp = TAR + '.tmp%d' % os.getpid()
        jobs = int(os.environ.get('JOBS') or max(1, (os.cpu_count() or 2) - 1))
        index = []
        with tarfile.open(tmp, 'w', format=tarfile.GNU_FORMAT) as tf, concurrent.futures.ThreadPoolExecutor(jobs) as ex:
            futs = [ex.submit(prep, k, src) for k, src in enumerate(srcs)]
            for k, (src, f) in enumerate(zip(srcs, futs)):
                base = f.result()
                work = os.path.join(wk, '%04d' % k)
                index.append('%d\t%s\t%s' % (k, src, '-' if base is None else base))
                if base is not None:
                    names = []
                    for dp, dn, fn in os.walk(work):
                        dn.sort()
                        names += [os.path.join(dp, x) for x in dn + sorted(fn)]
                    tf.add(work, arcname='%04d' % k, recursive=False, filter=norm)
                    for p in sorted(names):
                        tf.add(p, arcname='%04d/%s' % (k, os.path.relpath(p, work)), recursive=False, filter=norm)
                shutil.rmtree(work, ignore_errors=True)
            data = ('\n'.join(index) + '\n').encode()
            ti = tarfile.TarInfo('INDEX')
            ti.size = len(data)
            tf.addfile(norm(ti), io.BytesIO(data))
        os.replace(tmp, TAR)
        shutil.rmtree(wk, ignore_errors=True)
        print('%s: corpus.tar made: %d inputs, %.1f s' % (CFG.NAME, len(srcs), time.monotonic() - t0), flush=True)


class Tar:
    """the prepared inputs: cases() = [(k, label, base or None)], unpack(k, dir)"""
    def __init__(self):
        self.fd = os.open(TAR, os.O_RDONLY)
        self.members = {}
        with tarfile.open(TAR) as tf:
            for m in tf.getmembers():
                if m.name == 'INDEX':
                    self.index = tf.extractfile(m).read().decode().splitlines()
                else:
                    self.members.setdefault(int(m.name[:4]), []).append(m)

    def cases(self):
        out = []
        for line in self.index:
            k, label, base = line.split('\t')
            out.append((int(k), label, None if base == '-' else base))
        return out

    def unpack(self, k, work):
        for m in self.members.get(k, []):
            p = os.path.join(work, m.name[5:]) if len(m.name) > 4 else work
            if m.isdir():
                os.makedirs(p, exist_ok=True)
            elif m.isfile():
                with open(p, 'wb') as f:
                    f.write(os.pread(self.fd, m.size, m.offset_data))
                os.chmod(p, m.mode)
            elif m.issym():
                os.symlink(m.linkname, p)


def cache_read(path):
    try:
        with open(path, 'rb') as f:
            return pickle.loads(zlib.decompress(f.read()))
    except (OSError, zlib.error, pickle.PickleError, EOFError):
        return None


def cache_write(path, obj, prefix):
    """write atomically; older files of the same kind (prefix) go"""
    os.makedirs(CACHE, exist_ok=True)
    tmp = path + '.tmp%d' % os.getpid()
    with open(tmp, 'wb') as f:
        f.write(zlib.compress(pickle.dumps(obj), 6))
    os.replace(tmp, path)
    for f in os.listdir(CACHE):
        if f.startswith(prefix) and os.path.join(CACHE, f) != path and '.tmp' not in f:
            os.remove(os.path.join(CACHE, f))


def candidates(only, skip=False):
    """{name: path of the binary} of the candidates to test"""
    c = getattr(CFG, 'CANDIDATES', None) or {'wega': os.path.join(CFG.SRCDIR, CFG.CORPUS_BINARY)}
    legacy = os.environ.get('CORPUS_BIN')
    res = {}
    for name, path in c.items():
        if only and name not in only:
            continue
        path = os.environ.get('CORPUS_BIN_' + name.upper()) or (legacy if legacy and name == 'wega' else None) or path
        res[name] = path if os.path.isabs(path) else os.path.join(CFG.REPO, path)
    for name in only or []:
        if name not in c and not skip:
            sys.exit('%s: no candidate %s (progcfg CANDIDATES: %s)' % (CFG.NAME, name, ', '.join(c)))
    return res


def main():
    a = sys.argv[1:]
    keep = '-k' in a
    a = [x for x in a if x != '-k']
    n = flt = only = None
    if '--make' in a:
        if getattr(CFG, 'CORPUS_OF', None):
            sys.exit('%s: uses the tar of %s' % (CFG.NAME, CFG.CORPUS_OF))
        shutil.rmtree(CACHE, ignore_errors=True)
        make_tar(excl=True)
        return 0
    if '--ensure' in a:
        if not os.path.exists(TAR):
            make_tar()
        return 0
    showkeys = '--keys' in a
    a = [x for x in a if x != '--keys']
    if '-n' in a:
        i = a.index('-n'); n = int(a[i + 1]); del a[i:i + 2]
    if '-f' in a:
        i = a.index('-f'); flt = a[i + 1]; del a[i:i + 2]
    skip = '-s' in a
    a = [x for x in a if x != '-s']
    if '-c' in a:
        i = a.index('-c'); only = a[i + 1].split(','); del a[i:i + 2]
    cands = candidates(only, skip)
    if not cands:
        print('%s: no candidate %s' % (CFG.NAME, ','.join(only)))
        return 0
    for name, path in cands.items():
        if not os.path.exists(path):
            sys.exit('%s: %s (%s) not built' % (CFG.NAME, path, name))
    wall0 = time.monotonic()
    if not os.path.exists(TAR):
        if getattr(CFG, 'CORPUS_OF', None):	# the program that owns the tar makes it (it waits for the lock when that is under way)
            subprocess.run([sys.executable, os.path.abspath(__file__), '--ensure'], env=dict(os.environ, RECON_PROG=CFG.CORPUS_OF), check=True)
        else:
            make_tar()
    tar = Tar()
    exclude = getattr(CFG, 'CORPUS_EXCLUDE', lambda label: False)
    cases = [c for c in tar.cases() if not exclude(c[1])]
    excluded = len(tar.cases()) - len(cases)
    sources_t = time.monotonic() - wall0	# the tar and its index
    full = not flt and not n
    if flt:
        cases = [c for c in cases if flt in c[1]]
    if n:
        cases = cases[:n]
    parts = key_parts(cands)
    if showkeys:
        for kk, v in parts.items():
            print('%-60s %s' % (kk, v))
        return 0
    okey = mix({kk: v for kk, v in parts.items() if not kk.startswith(('binary', 'depends'))})
    ckey = {c: mix(dict(okey=okey, **{kk: v for kk, v in parts.items() if kk.startswith(('binary %s' % c, 'depends %s ' % c))}))
            for c in cands}
    nocache = bool(os.environ.get('CORPUS_NOCACHE'))
    okfile = {c: os.path.join(CACHE, 'ok-%s-%s.%s' % (CFG.NAME, c, ckey[c])) for c in cands}
    same = {c: 0 for c in cands}
    total = len(cases)
    todo = dict(cands)
    for c in cands:
        if not nocache and full and os.path.exists(okfile[c]):
            print(open(okfile[c]).read().strip() + ' (cached)', flush=True)
            del todo[c]
    if not todo:
        print('TIME %-8s wall %7.1f (cached, key %s)' % (CFG.NAME, time.monotonic() - wall0, okey))
        return 0
    cands = todo
    origfile = os.path.join(CACHE, 'orig-%s.%s' % (CFG.NAME, okey))
    ocache = None if nocache else cache_read(origfile)
    ocache_new = {}
    shutil.rmtree(WORK, ignore_errors=True)
    os.makedirs(WORK)

    def case(k, label, base):
        """(skipped, {candidate: differences}) of one input, in a directory of its own"""
        t0 = time.monotonic()
        if base is None:
            return True, {}
        work = os.path.join(WORK, '%04d' % k)
        os.makedirs(work)
        tar.unpack(k, work)
        t1 = time.monotonic()
        phase_add('prepare', t1 - t0)
        diffs = {c: [] for c in cands}
        for name, path in cands.items():
            shutil.copy(path, os.path.join(work, 'pass_' + name))
            os.makedirs(os.path.join(work, '.prog_' + name))
            shutil.copy(path, os.path.join(work, '.prog_' + name, CFG.NAME))
        for vi, v in enumerate(VARIANTS):
            ta = time.monotonic()
            r1 = ocache.get((k, vi)) if ocache else None
            if r1 is None:
                r1 = run_pass('/' + CFG.ORIG_BIN, base, work, v, 'orig')	# the original once for all candidates
                ocache_new[(k, vi)] = r1
            phase_add('original', time.monotonic() - ta)
            tag = '' if v is None else '[%s] ' % (' '.join('%s=%s' % kv for kv in v.items()) if isinstance(v, dict) else ' '.join(v))
            for name in cands:
                ta = time.monotonic()
                r2 = run_pass('pass_' + name, base, work, v, name, native=(name == 'host'))
                phase_add(name, time.monotonic() - ta)
                d = diffs[name]
                if r1[0] != r2[0]:
                    d.append('%src %d/%d' % (tag, r1[0], r2[0]))
                if r1[1] != r2[1]:
                    d.append(tag + 'messages')
                for f in r1[2]:
                    if r1[2][f] != r2[2][f]:
                        d.append('%s%s %s/%s' % (tag, f, None if r1[2][f] is None else len(r1[2][f]),
                                                 None if r2[2][f] is None else len(r2[2][f])))
        if not (any(diffs.values()) and keep):
            shutil.rmtree(work)
        phase_add('total', time.monotonic() - t0)
        return False, diffs

    total = skipped = 0
    same = {c: 0 for c in cands}
    jobs = int(os.environ.get('JOBS') or max(1, (os.cpu_count() or 2) - 1))
    with concurrent.futures.ThreadPoolExecutor(jobs) as ex:
        futs = [ex.submit(case, *c) for c in cases]
        for c, f in zip(cases, futs):
            skip, diffs = f.result()
            if skip:
                skipped += 1
                continue
            total += 1
            for cn in cands:
                if diffs[cn]:
                    print('DIFF [%s] %s: %s' % (cn, c[1], ', '.join(diffs[cn])), flush=True)
                else:
                    same[cn] += 1
    skipped += excluded
    for c in cands:
        line = '%s [%s]: %d of %d inputs identical (%d sources skipped)' % (CFG.NAME, c, same[c], total, skipped)
        print(line, flush=True)
        if full and same[c] == total and total > 0:
            os.makedirs(CACHE, exist_ok=True)
            for f in os.listdir(CACHE):
                if f.startswith('ok-%s-%s.' % (CFG.NAME, c)):
                    os.remove(os.path.join(CACHE, f))
            with open(okfile[c], 'w') as f:
                f.write(line + '\n')
    if full and ocache_new and ocache is None:
        cache_write(origfile, ocache_new, 'orig-%s.' % CFG.NAME)
    # TIME: the wall time and the CPU seconds of the phases (over all cases; 'rest' = copies, comparison, clean up)
    ph = lambda n: PHASES.get(n, 0.0)
    used = ph('prepare') + ph('original') + sum(ph(c) for c in cands)
    print('TIME %-8s wall %7.1f (tar %5.1f)  cpu: unpack %7.1f original %7.1f%s %s rest %6.1f   %d cases, %d at a time' % (
        CFG.NAME, time.monotonic() - wall0, sources_t, ph('prepare'), ph('original'), ' (cache)' if ocache else '',
        ' '.join('%s %7.1f' % (c, ph(c)) for c in cands), max(0.0, ph('total') - used), total + skipped, jobs))
    return 1 if any(same[c] != total for c in cands) else 0


if __name__ == '__main__':
    sys.exit(main())
