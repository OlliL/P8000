#!/usr/bin/env python3
"""corpusrun.py - the corpus test of a program (increment 110): the original of the WEGA tree and every candidate on the same
cases, compared. Usage: corpusrun.py [-c CANDIDATE[,CANDIDATE]] [-k] [-f FILTER] [--nocache] NAME|DIR   (normally host/tools/corpus.sh)

A corpus is compare/NAME/corpus.py (NAME, ORIG, PROG, CANDIDATES, optional ROOT_FILES, DEPENDS, generate(), normalize()) and
the hand-written cases compare/NAME/cases/CASE/ (a file `case`, the starting files in `in/`). The case file (one field per line,
# a comment line):
  step: WORDS         a run of a WEGA program under the emulator; first word PROG: the program under test (the original, or
                      a candidate), another: /bin/WORD of the WEGA tree (the original for both sides); shell-like words, no
                      shell operators
  stdin: FILE         stdin of the step before (a file of the case directory after in/)
  env: NAME=VALUE     environment of the step before, or of every step when it comes before the first one
  host: LINE          a line of the host shell between the steps (preparation; WEGA_ROOT is set; each line its own shell)
Every step and host line runs with umask 022.
  variant: N=V ...    the case runs once per variant line with that environment (default: once, none)
  expect: exit N | output contains TEXT | file PATH contains TEXT | file PATH same-as in/FILE | missing PATH
                      checked on the original's run
Compared: per step its output (stdout, stderr) and exit status, afterwards every entry of the directory: type, mode, size,
checksum, link count (not owner and times).

Store (WEGA/.corpus, or CORPUS_DB): results/orig/KEY (the original's result; computed twice, kept only when both are
equal), results/ok/KEY (a candidate was identical), locks/. Keys: case (case file and in/ by names and contents) + variant +
original binary + ROOT_FILES + emulator + this runner -> original; + candidate binary + DEPENDS -> candidate. Written by rename;
one run computes a key at a time (flock).
Output: one line per differing case (with the differing lines), then per candidate "NAME [CAND]: N of M inputs identical";
exit status 1 when a case differs, an expectation fails or the original is unstable.
"""
import base64
import concurrent.futures
import fcntl
import hashlib
import importlib.util
import json
import os
import shlex
import shutil
import subprocess
import sys
import threading
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
ROOT = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or os.path.normpath(os.path.join(REPO, '..', 'tmp', 'wega31_root'))
EMU = os.environ.get('EMU') or os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu')
STORE = os.environ.get('CORPUS_DB') or os.path.join(REPO, '.corpus')
WORK = os.path.join(os.environ.get('STAGE_TMP', '/tmp/wega_stage'), 'corpusrun')
os.environ.setdefault('WEGA_ROOT', ROOT)

_SHA = {}


def sha1file(p):
    if p not in _SHA:
        h = hashlib.sha1()
        with open(p, 'rb') as f:
            for b in iter(lambda: f.read(1 << 20), b''):
                h.update(b)
        _SHA[p] = h.hexdigest()
    return _SHA[p]


def sha1tree(p):
    if os.path.isfile(p):
        return sha1file(p)
    h = hashlib.sha1()
    for dp, dn, fn in os.walk(p):
        dn.sort()
        for f in sorted(fn):
            q = os.path.join(dp, f)
            h.update(os.path.relpath(q, p).encode() + b'\0' + (sha1file(q) if os.path.isfile(q) else 'x').encode())
    return h.hexdigest()


def mix(*parts):
    return hashlib.sha1('\n'.join(parts).encode()).hexdigest()


# ---- the case -----------------------------------------------------------------------------------------------------------

class CaseError(Exception):
    pass


def parse_case(text, where):
    """{'env': {..}, 'steps': [{'kind': 'step'|'host', 'argv'|'line', 'stdin', 'env'}], 'variants': [{..}], 'expect': [..]}"""
    c = {'env': {}, 'steps': [], 'variants': [], 'expect': []}
    for n, line in enumerate(text.split('\n'), 1):
        line = line.rstrip()
        if not line.strip() or line.lstrip().startswith('#'):
            continue
        if ':' not in line:
            raise CaseError('%s:%d: no field' % (where, n))
        k, v = line.split(':', 1)
        k, v = k.strip(), v.strip()
        if k == 'step':
            argv = shlex.split(v)
            if not argv:
                raise CaseError('%s:%d: empty step' % (where, n))
            c['steps'].append({'kind': 'step', 'argv': argv, 'stdin': None, 'env': {}})
        elif k == 'host':
            c['steps'].append({'kind': 'host', 'line': v})
        elif k == 'stdin':
            if not c['steps'] or c['steps'][-1]['kind'] != 'step':
                raise CaseError('%s:%d: stdin without a step' % (where, n))
            c['steps'][-1]['stdin'] = v
        elif k == 'env':
            name, _, val = v.partition('=')
            if c['steps']:
                if c['steps'][-1]['kind'] != 'step':
                    raise CaseError('%s:%d: env after a host line' % (where, n))
                c['steps'][-1]['env'][name] = val
            else:
                c['env'][name] = val
        elif k == 'variant':
            c['variants'].append(dict(w.split('=', 1) for w in shlex.split(v)))
        elif k == 'expect':
            c['expect'].append(shlex.split(v))
        else:
            raise CaseError('%s:%d: unknown field %s' % (where, n, k))
    if not c['steps']:
        raise CaseError('%s: no step' % where)
    for e in c['expect']:
        ok = (e[:1] == ['exit'] and len(e) == 2) or (e[:2] == ['output', 'contains'] and len(e) == 3) or \
             (e[:1] == ['file'] and len(e) == 4 and e[2] in ('contains', 'same-as')) or (e[:1] == ['missing'] and len(e) == 2)
        if not ok:
            raise CaseError('%s: unknown expect %s' % (where, ' '.join(e)))
    return c


def case_hash(d):
    h = hashlib.sha1()
    for dp, dn, fn in os.walk(d):
        dn.sort()
        for f in sorted(fn):
            q = os.path.join(dp, f)
            h.update(os.path.relpath(q, d).encode() + b'\0' + sha1file(q).encode() + b'\n')
    return h.hexdigest()


# ---- running ------------------------------------------------------------------------------------------------------------

# copying a program and starting a process exclude each other (see stagecorpus.py: ETXTBSY of a copy whose write
# descriptor a fork of another thread holds until its exec)
SPAWN = threading.Lock()


def spawn(args, cwd, env, stdin=None, timeout=600):
    with SPAWN:
        p = subprocess.Popen(args, cwd=cwd, env=env, stdin=stdin or subprocess.DEVNULL, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
    try:
        out, _ = p.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        p.kill()
        p.communicate()
        return -999, b'timeout\n'
    return p.returncode, out


def listing(d):
    """every entry below d (not .prog): type, mode, size, checksum, link count"""
    out = []
    for dp, dn, fn in os.walk(d):
        if dp == d:
            dn[:] = [x for x in dn if x != '.prog']
        dn.sort()
        for name in sorted(dn) + sorted(fn):
            p = os.path.join(dp, name)
            st = os.lstat(p)
            rel = os.path.relpath(p, d)
            if os.path.islink(p):
                out.append('l %s -> %s' % (rel, os.readlink(p)))
            elif os.path.isdir(p):
                out.append('d %o %s' % (st.st_mode & 0o7777, rel))
            else:
                data = open(p, 'rb').read()
                out.append('f %o %d %08x %d %s' % (st.st_mode & 0o7777, len(data), zlib.crc32(data), st.st_nlink, rel))
    return '\n'.join(out)


def run_case(cfg, cdir, case, variant, side, binary, rd):
    """one run of the case: side 'orig', or a candidate name with its binary; the result as a dict of strings"""
    shutil.rmtree(rd, ignore_errors=True)
    if os.path.isdir(os.path.join(cdir, 'in')):
        shutil.copytree(os.path.join(cdir, 'in'), rd, symlinks=True)
    else:
        os.makedirs(rd)
    native = side == 'host'
    if side != 'orig':
        os.makedirs(os.path.join(rd, '.prog'))
        with SPAWN:
            shutil.copy(binary, os.path.join(rd, '.prog', cfg.PROG))
    env = dict(os.environ)
    env.pop('TZ', None)
    env['LC_ALL'] = 'C'
    genv = dict(case['env'])
    genv.update(variant)
    res = {'steps': []}
    for i, st in enumerate(case['steps']):
        if st['kind'] == 'host':
            e = dict(env)
            e['WEGA_ROOT'] = ROOT
            rc, out = spawn(['sh', '-c', 'umask 022; ' + st['line']], rd, e)
            res['steps'].append('host %d: exit=%d\n%s' % (i, rc, out.decode('latin-1')))
            continue
        argv = st['argv']
        senv = dict(genv)
        senv.update(st['env'])
        if argv[0] == cfg.PROG and native:
            e = dict(env)
            e.update(senv)
            e.update(getattr(cfg, 'HOST_ENV', {}))
            args = [os.path.join(rd, '.prog', cfg.PROG)] + argv[1:]
        else:
            if argv[0] == cfg.PROG:
                prog = '/' + cfg.ORIG if side == 'orig' else '.prog/' + cfg.PROG
            else:
                prog = '/bin/' + argv[0]
            args = [EMU]
            for k, v in senv.items():
                args += ['-e', '%s=%s' % (k, v)]
            args += ['-r', ROOT, prog] + argv[1:]
            e = env
        args = ['sh', '-c', 'umask 022; exec "$@"', 'sh'] + args	# every step with umask 022 (the modes of new files)
        fin = open(os.path.join(rd, st['stdin']), 'rb') if st['stdin'] else None
        try:
            rc, out = spawn(args, rd, e, fin)
        finally:
            if fin:
                fin.close()
        text = out.decode('latin-1')
        norm = getattr(cfg, 'normalize', None)
        if norm:
            text = norm(case, 'output', text)
        res['steps'].append('step %d %s: exit=%d\n%s' % (i, argv[0], rc, text))
    res['tree'] = listing(rd)
    return res


def check_expect(case, res, rd):
    bad = []
    last = [s for s in res['steps'] if s.startswith('step ')]
    for e in case['expect']:
        if e[0] == 'exit':
            if not last or not last[-1].split('\n', 1)[0].endswith('exit=%s' % e[1]):
                bad.append('exit %s' % e[1])
        elif e[0] == 'output':
            if not any(e[2] in s for s in res['steps']):
                bad.append('output contains %s' % e[2])
        elif e[0] == 'file':
            p = os.path.join(rd, e[1])
            if not os.path.isfile(p):
                bad.append('file %s (missing)' % e[1])
            elif e[2] == 'contains':
                if e[3].encode() not in open(p, 'rb').read():
                    bad.append('file %s contains %s' % (e[1], e[3]))
            else:
                ref = os.path.join(case['_dir'], e[3])
                if not os.path.isfile(ref) or open(ref, 'rb').read() != open(p, 'rb').read():
                    bad.append('file %s same-as %s' % (e[1], e[3]))
        elif e[0] == 'missing':
            if os.path.lexists(os.path.join(rd, e[1])):
                bad.append('missing %s' % e[1])
    return bad


def explain(r1, r2, n=12):
    import difflib
    out = []
    a = '\n'.join(r1['steps']) + '\n--tree--\n' + r1['tree']
    b = '\n'.join(r2['steps']) + '\n--tree--\n' + r2['tree']
    dl = [l for l in difflib.unified_diff(a.split('\n'), b.split('\n'), lineterm='', n=0) if l[:1] in '-+' and l[:3] not in ('---', '+++')]
    out.append(' | '.join(dl[:n]) + (' ...' if len(dl) > n else ''))
    return out


# ---- the store ----------------------------------------------------------------------------------------------------------

def store_path(kind, key):
    return os.path.join(STORE, 'results', kind, key[:2], key)


def store_read(kind, key):
    try:
        with open(store_path(kind, key), 'rb') as f:
            os.utime(store_path(kind, key))
            return json.loads(zlib.decompress(f.read()))
    except (OSError, ValueError, zlib.error):
        return None


def store_write(kind, key, obj):
    p = store_path(kind, key)
    os.makedirs(os.path.dirname(p), exist_ok=True)
    tmp = '%s.tmp%d.%d' % (p, os.getpid(), threading.get_ident())
    with open(tmp, 'wb') as f:
        f.write(zlib.compress(json.dumps(obj).encode(), 6))
    os.replace(tmp, p)


class KeyLock:
    def __init__(self, key):
        d = os.path.join(STORE, 'locks')
        os.makedirs(d, exist_ok=True)
        self.f = open(os.path.join(d, key), 'w')

    def __enter__(self):
        fcntl.flock(self.f, fcntl.LOCK_EX)
        return self

    def __exit__(self, *a):
        self.f.close()


# ---- the corpus ---------------------------------------------------------------------------------------------------------

def load(name):
    d = name if os.path.isdir(name) and os.path.exists(os.path.join(name, 'corpus.py')) else os.path.join(REPO, 'compare', name)
    spec = importlib.util.spec_from_file_location('corpus_' + os.path.basename(d), os.path.join(d, 'corpus.py'))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    m.DIR = d
    return m


def cases(cfg):
    out = []
    cd = os.path.join(cfg.DIR, 'cases')
    if os.path.isdir(cd):
        for n in sorted(os.listdir(cd)):
            if os.path.isfile(os.path.join(cd, n, 'case')):
                out.append((n, os.path.join(cd, n)))
    return out


def runner_key():
    return mix(sha1file(os.path.abspath(__file__)))


def main():
    a = sys.argv[1:]
    cands_only = None
    keep = nocache = False
    filt = None
    while a and a[0].startswith('-'):
        o = a.pop(0)
        if o == '-c':
            cands_only = a.pop(0).split(',')
        elif o == '-k':
            keep = True
        elif o == '-f':
            filt = a.pop(0)
        elif o == '--nocache':
            nocache = True
        else:
            print(__doc__)
            return 2
    nocache = nocache or bool(os.environ.get('CORPUS_NOCACHE'))
    cfg = load(a[0])
    cands = {}
    for c, p in cfg.CANDIDATES.items():
        if cands_only and c not in cands_only:
            continue
        path = os.environ.get('CORPUS_BIN_' + c.upper()) or os.path.join(REPO, p)
        if not os.path.exists(path):
            print('%s: %s (%s) not built' % (cfg.NAME, path, c))
            return 1
        cands[c] = path
    base = [sha1file(os.path.join(ROOT, cfg.ORIG)), sha1file(EMU), runner_key()]
    base += [f + ' ' + (sha1tree(os.path.join(ROOT, f)) if os.path.exists(os.path.join(ROOT, f)) else 'missing')
             for f in getattr(cfg, 'ROOT_FILES', [])]
    work = os.path.join(WORK, cfg.NAME)
    shutil.rmtree(work, ignore_errors=True)
    os.makedirs(work)
    jobs = []
    for cname, cdir in cases(cfg):
        if filt and filt not in cname:
            continue
        try:
            case = parse_case(open(os.path.join(cdir, 'case')).read(), os.path.join(cdir, 'case'))
        except CaseError as e:
            print('CASE %s' % e)
            return 1
        case['_dir'] = cdir
        ch = case_hash(cdir)
        for vi, var in enumerate(case['variants'] or [{}]):
            jobs.append((cname, cdir, case, vi, var, mix(*([ch, json.dumps(var, sort_keys=True)] + base))))
    t0 = time.monotonic()
    same = {c: 0 for c in cands}
    bad = []

    def one(job):
        cname, cdir, case, vi, var, okey = job
        label = cname + ('[%s]' % ' '.join('%s=%s' % kv for kv in var.items()) if var else '')
        rdir = os.path.join(work, '%s.%d' % (cname, vi))
        msgs = []
        r1 = None if nocache else store_read('orig', okey)
        if r1 is None:
            with KeyLock(okey):
                r1 = None if nocache else store_read('orig', okey)
                if r1 is None:
                    ra = run_case(cfg, cdir, case, var, 'orig', None, rdir + '.orig')
                    rb = run_case(cfg, cdir, case, var, 'orig', None, rdir + '.orig2')
                    if ra != rb:
                        return label, ['UNSTABLE original: ' + explain(ra, rb)[0]], {c: False for c in cands}
                    shutil.rmtree(rdir + '.orig2', ignore_errors=True)
                    r1 = ra
                    r1['expect'] = check_expect(case, ra, rdir + '.orig')
                    store_write('orig', okey, r1)
        if r1.get('expect'):
            msgs.append('EXPECT failed on the original: ' + '; '.join(r1['expect']))
        res = {}
        for c, path in cands.items():
            ckey = mix(okey, sha1file(path), *[sha1tree(os.path.join(REPO, d)) for d in getattr(cfg, 'DEPENDS', {}).get(c, [])])
            if not nocache and os.path.exists(store_path('ok', ckey)):
                res[c] = not msgs
                continue
            r2 = run_case(cfg, cdir, case, var, c, path, rdir + '.' + c)
            r1c = {'steps': r1['steps'], 'tree': r1['tree']}
            if r1c == r2:
                store_write('ok', ckey, 1)
                res[c] = not msgs
                if not keep:
                    shutil.rmtree(rdir + '.' + c, ignore_errors=True)
            else:
                res[c] = False
                msgs.append('[%s] %s' % (c, explain(r1c, r2)[0]))
        if not keep:
            shutil.rmtree(rdir + '.orig', ignore_errors=True)
        return label, msgs, res

    n = int(os.environ.get('JOBS') or max(1, (os.cpu_count() or 2) - 1))
    with concurrent.futures.ThreadPoolExecutor(n) as ex:
        for label, msgs, res in ex.map(one, jobs):
            for c, ok in res.items():
                if ok:
                    same[c] += 1
            if msgs:
                bad.append(label)
                print('DIFF %s: %s' % (label, '\n    '.join(msgs)), flush=True)
    for c in cands:
        print('%s [%s]: %d of %d inputs identical (0 sources skipped)' % (cfg.NAME, c, same[c], len(jobs)), flush=True)
    print('TIME %-8s wall %7.1f   %d cases' % (cfg.NAME, time.monotonic() - t0, len(jobs)))
    return 1 if bad or any(same[c] != len(jobs) for c in cands) else 0


if __name__ == '__main__':
    sys.exit(main())
