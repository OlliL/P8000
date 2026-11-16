#!/usr/bin/env python3
"""corpusrun.py - the corpus test of a program (increment 110): the original of the WEGA tree and every candidate on the same
cases, compared. Usage: corpusrun.py [-c CANDIDATE[,CANDIDATE]] [-k] [-f FILTER] [--nocache] [--show] NAME|DIR   (normally
host/tools/corpus.sh). --show: only the original, every case printed (steps with output and exit status, the files
afterwards, the expectations that fail); no store, no candidate: for writing cases.

A corpus is compare/NAME/corpus.py (NAME, ORIG, PROG, CANDIDATES, optional ROOT_FILES, ROOT_ENV, ALIASES, DEPENDS, HOST_ENV, TIMEOUT (seconds per step, default 600), IN_TMP (the case
directory is the guest's /tmp/CASE.N.SIDE, its parent the guest /tmp: for programs that find their directory by pwd), host_setup(),
generate(), normalize(), after_step()) and
the hand-written cases compare/NAME/cases/CASE/ (a file `case`, the starting files in `in/`). The case file (one field per line,
# a comment line):
  step: WORDS         a run of a WEGA program under the emulator; first word PROG (or a name of ALIASES of the corpus, e.g.
                      cp, ln for mv: {word: path of the original}): the program under test (the original, or the candidate
                      under that name, beside the case directory), another: /bin/WORD of the WEGA tree (the original for
                      both sides); argv[0] is the word (z8k-emu -0), as a shell gives it; shell-like words, no
                      shell operators; a word with * ? [ is expanded in the case directory as by the shell, $V by the
                      words of the variant
  stdin: FILE         stdin of the step before (a file of the case directory after in/)
  env: NAME=VALUE     environment of the step before, or of every step when it comes before the first one
  host: LINE          a line of the host shell between the steps (preparation; WEGA_ROOT and WEGA_HOST (WEGA/host) are
                      set; each line its own shell)
Every step and host line runs with umask 022.
  terminal: console   the minimal terminal (z8k-emu Z8K_CONSOLE): stdin/out of the steps are the console for isatty,
                      ttyname, the tty ioctls; /dev/console and /dev/tty are it
  device: driveN FILE | ud | NAME KIND:ARG [excl] [console]   the guest /dev (z8k-emu Z8K_DEVTAB; write_devtab below):
                      a floppy image of the case directory as drive N, the 8-bit part behind /dev/ud, any device model
  user: time: fs: signal:   reserved for the surroundings; an error until the emulator serves them
  variant: N=V ...    the case runs once per variant line with that environment (default: once, none)
  expect: exit N | output contains TEXT | output is TEXT (the whole output of the last step, \n a newline) | file PATH contains TEXT | file PATH same-as in/FILE|PATH | missing PATH
                      checked on the original's run
Compared: per step its output (stdout, then stderr apart; after_step(argv, rc, out, err, dir) of the corpus may cut what the
original leaves undefined) and exit status, afterwards every entry of the directory: type, mode, size,
checksum, link count (not owner and times).

The files of the tree (ROOT) a run or a preparation reads through the emulator are measured (Z8K_ROOTLOG) and kept with
the result: a result or prepared input whose files have changed since is made again; the candidate's key holds them too.
ROOT_FILES and prep_files remain for files read outside the emulator.
Store (WEGA/.corpus, or CORPUS_DB): results/orig/KEY (the original's result; computed twice, kept only when both are
equal), results/ok/KEY (a candidate was identical), locks/. Keys: case (case file and in/ by names and contents) + variant +
original binary + ROOT_FILES + emulator + this runner -> original; + candidate binary + DEPENDS -> candidate. Written by rename;
one run computes a key at a time (flock).
Output: one line per differing case (with the differing lines), then per candidate "NAME [CAND]: N of M inputs identical";
exit status 1 when a case differs, an expectation fails or the original is unstable.
"""
import base64
import re
import concurrent.futures
import fcntl
import glob
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
WORK = os.path.join(os.environ.get('CORPUS_TMP', '/tmp/wega_corpus'), 'corpusrun')
os.environ.setdefault('WEGA_ROOT', ROOT)
# every emulator writes the guest paths it looks up below its ROOT to this file of its starting directory (Z8K_ROOTLOG):
# the measured dependencies of a run or a preparation on the WEGA tree
ROOTLOG = '.z8k_rootlog'
os.environ['Z8K_ROOTLOG'] = ROOTLOG
RUNROOT = ROOT		# the tree the original and the WEGA programs of the steps run in (ROOT_ENV of the corpus, else ROOT)

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


# the fields of the surroundings of a case the format reserves; each is an error until the emulator serves it
LATER = {
    'user': 'the user and group ids the program runs as (and the files of /etc/passwd, /etc/group it sees)',
    'time': 'the time the guest sees (the emulator has Z8K_TIME for a fixed time; the field comes with the first program that needs it)',
    'fs': 'a file system of its own (mount, a block device with a WEGA file system)',
    'signal': 'a signal sent to the step after N seconds or at a text of its output',
}


def parse_case(text, where):
    """{'env': {..}, 'steps': [{'kind': 'step'|'host', 'argv'|'line', 'stdin', 'env'}], 'variants': [{..}], 'expect': [..]}"""
    c = {'env': {}, 'steps': [], 'variants': [], 'expect': [], 'devices': [], 'terminal': None}
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
            ws = shlex.split(v)
            env = [w for w in ws if re.match(r'[A-Za-z_][A-Za-z0-9_]*=', w)]
            c['variants'].append({'env': dict(w.split('=', 1) for w in env), 'words': [w for w in ws if w not in env]})
        elif k == 'expect':
            c['expect'].append(shlex.split(v))
        elif k == 'terminal':
            if v != 'console':
                raise CaseError('%s:%d: terminal: only "console" (the minimal terminal) so far' % (where, n))
            c['terminal'] = v
        elif k == 'device':
            ws = shlex.split(v)
            if not ws or not (re.match(r'drive[0-3]$', ws[0]) and len(ws) == 2 or ws[0] == 'ud' and len(ws) == 1 or
                              len(ws) >= 2 and ':' in ws[1]):
                raise CaseError('%s:%d: device: driveN FILE | ud | NAME KIND:ARG [excl] [console]' % (where, n))
            c['devices'].append(ws)
        elif k in LATER:
            raise CaseError('%s:%d: %s: %s; not served yet (the emulator has no model of it; increment 120 builds it with the '
                            'first program that needs it)' % (where, n, k, LATER[k]))
        else:
            raise CaseError('%s:%d: unknown field %s' % (where, n, k))
    if not c['steps']:
        raise CaseError('%s: no step' % where)
    for e in c['expect']:
        ok = (e[:1] == ['exit'] and len(e) == 2) or (e[:1] == ['output'] and e[1:2] in (['contains'], ['is']) and len(e) == 3) or \
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

# copying a program and starting a process exclude each other (ETXTBSY: a fork of another thread holds the write descriptor
# of a copy until its exec)
SPAWN = threading.Lock()


def spawn(args, cwd, env, stdin=None, timeout=600):
    with SPAWN:
        p = subprocess.Popen(args, cwd=cwd, env=env, stdin=stdin or subprocess.DEVNULL, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE)
    try:
        out, err = p.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        p.kill()
        p.communicate()
        return -999, b'timeout\n', b''
    return p.returncode, out, err


def joined(out, err):
    """stdout, then stderr apart (the order of the two in one pipe depends on the buffering of the C library)"""
    return out + ('-- stderr\n' + err if err else '')


def listing(d, norm=None):
    """every entry below d: type, mode, size, checksum (of the file as norm(name, data) gives it), link count"""
    out = []
    for dp, dn, fn in os.walk(d):
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
                if norm:
                    data = norm(rel, data)
                out.append('f %o %d %08x %d %s' % (st.st_mode & 0o7777, len(data), zlib.crc32(data), st.st_nlink, rel))
    return '\n'.join(out)


def _read_rootlog(p, paths):
    paths.update(l for l in open(p, encoding='latin-1').read().split('\n')
                 if l and not l.startswith(('/tmp/', '/dev/')) and l != '/tmp')
    os.remove(p)


DEVTAB = os.path.join(REPO, 'host', 'z8k-emu', 'devtab.wega')
UD8 = os.path.join(REPO, 'host', 'z8k-emu', 'models', 'ud8.py')


def write_devtab(case, rd):
    """the guest /dev of a run (z8k-emu Z8K_DEVTAB): WEGA's /dev (devtab.wega) and the models the case names:
    device: driveN FILE   every floppy device of drive N (fd(4): minor & 3) reads and writes the image FILE
    device: ud            /dev/ud is the 8-bit part (models/ud8.py) with the drives of the case; what the 8-bit part
                          prints on its console is recorded as part of the result
    device: NAME KIND:ARG [excl] [console]   any device with a model of z8k-emu (image:, host:, proc:, console)
    terminal: console     the minimal terminal: fds 0-2, /dev/console and /dev/tty are the console"""
    tab = open(DEVTAB).read()
    lines = ['model null host:/dev/null']
    if case.get('terminal') == 'console':
        lines += ['model console console', 'model tty console']
    drives = {}
    ents = []
    last = None
    for l in tab.split('\n'):
        w = l.split()
        if len(w) == 6 and w[0][0] in 'bc':
            last = [w[5], int(w[3]), int(w[4])]
            ents.append(last)
        elif len(w) == 2 and w[0] == 'l' and last:
            ents.append([w[1], last[1], last[2]])
    for ws in case.get('devices', []):
        if ws[0].startswith('drive'):
            n = int(ws[0][5:])
            img = os.path.join(rd, ws[1])
            drives[n] = img
            lines += ['model %s image:%s' % (name, img) for name, ma, mi in ents if ma == 1 and mi & 3 == n]
        elif ws[0] == 'ud':
            args = ''.join(' --drive%d %s' % (n, shlex.quote(p)) for n, p in sorted(drives.items()))
            lines.append('model ud proc:%s %s%s --console %s excl console' % (sys.executable, UD8, args,
                                                                             shlex.quote(rd + '.udcon')))
        else:
            kind, _, arg = ws[1].partition(':')
            if kind == 'image':
                arg = os.path.join(rd, arg)
            lines.append('model %s %s:%s %s' % (ws[0], kind, arg, ' '.join(ws[2:])))
    p = rd + '.devtab'
    with open(p, 'w') as f:
        f.write(tab + '\n'.join(lines) + '\n')
    return p


def collect_rootlog(d, extra=None):
    """the guest paths the emulators started below d (or writing to the file extra) looked up in their ROOT (the log
    files are removed); not /tmp, /dev"""
    paths = set()
    for dp, dn, fn in os.walk(d):
        if ROOTLOG in fn:
            _read_rootlog(os.path.join(dp, ROOTLOG), paths)
    if extra and os.path.exists(extra):
        _read_rootlog(extra, paths)
    return paths


_DEPMEMO = {}
_DEPLOCK = threading.Lock()


def dep_hash(root, g):
    """the state of the guest path g of the tree root: a file by its contents, a directory by its names, '-' when missing"""
    p = root + g
    try:
        st = os.stat(p)
    except OSError:
        return '-'
    k = (p, st.st_ino, st.st_mtime_ns, st.st_size)
    with _DEPLOCK:
        if k in _DEPMEMO:
            return _DEPMEMO[k]
    if os.path.isdir(p):
        h = 'd' + hashlib.sha1('\n'.join(sorted(os.listdir(p))).encode('latin-1', 'replace')).hexdigest()
    else:
        try:
            h = sha1file(p)
        except OSError:
            h = '?'
    with _DEPLOCK:
        _DEPMEMO[k] = h
    return h


def deps_of(root, paths):
    return {g: dep_hash(root, g) for g in sorted(paths)}


def deps_ok(root, deps):
    """the files of the tree a result was made from are as they were"""
    return all(dep_hash(root, g) == h for g, h in (deps or {}).items())


def run_case(cfg, indir, case, variant, side, binary, rd):
    """one run of the case (its starting files in indir): side 'orig', or a candidate name with its binary; the result as a
    dict of strings"""
    shutil.rmtree(rd, ignore_errors=True)
    if os.path.isdir(indir):
        shutil.copytree(indir, rd, symlinks=True)
    else:
        os.makedirs(rd)
    native = side == 'host'
    # the candidate lies beside the case directory, not in it (a program that reads its directory must not see it), under
    # every name of ALIASES (one binary, several names: mv, cp, ln)
    aliases = dict(getattr(cfg, 'ALIASES', {}))
    aliases.setdefault(cfg.PROG, cfg.ORIG)
    progdir = rd + '.prog'
    shutil.rmtree(progdir, ignore_errors=True)
    if side != 'orig':
        os.makedirs(progdir)
        with SPAWN:
            shutil.copy(binary, os.path.join(progdir, cfg.PROG))
        for a in aliases:
            if a != cfg.PROG:
                os.link(os.path.join(progdir, cfg.PROG), os.path.join(progdir, a))
        if native and hasattr(cfg, 'host_setup'):
            cfg.host_setup(progdir)
    env = dict(os.environ)
    env['Z8K_ROOTLOG'] = rd + '.rootlog'	# beside the case directory: a program that reads its directory must not see it
    if case.get('devices') or case.get('terminal'):
        env['Z8K_DEVTAB'] = write_devtab(case, rd)
        if case.get('terminal') == 'console':
            env['Z8K_CONSOLE'] = '1'
    if getattr(cfg, 'IN_TMP', False):	# the case directory is /tmp/NAME of the guest (pwd works: z8k-emu, Z8K_TMPDIR)
        env['Z8K_TMPDIR'] = os.path.dirname(rd)
    if os.path.exists(rd + '.rootlog'):
        os.remove(rd + '.rootlog')
    env.pop('TZ', None)
    env['LC_ALL'] = 'C'
    genv = dict(case['env'])
    genv.update(variant['env'])
    res = {'steps': []}
    for i, st in enumerate(case['steps']):
        if st['kind'] == 'host':
            e = dict(env)
            e['WEGA_ROOT'] = ROOT
            e['WEGA_HOST'] = os.path.join(REPO, 'host')	# the tools of the repository (e.g. $WEGA_HOST/tools/udosfs.py)
            rc, out, err = spawn(['sh', '-c', 'umask 022; ' + st['line']], rd, e)
            res['steps'].append('host %d: exit=%d\n%s' % (i, rc, joined(out.decode('latin-1'), err.decode('latin-1'))))
            continue
        argv = []
        for w in st['argv']:
            if w == '$V':
                argv += variant['words']
            elif any(c in w for c in '*?['):	# a pattern is expanded in the case directory as the shell does it
                m = sorted(os.path.relpath(x, rd) for x in glob.glob(os.path.join(rd, w)))
                argv += m or [w]
            else:
                argv.append(w)
        senv = dict(genv)
        senv.update(st['env'])
        if argv[0] in aliases and native:
            e = dict(env)
            e.update(senv)
            he = getattr(cfg, 'HOST_ENV', {})
            e.update(he(ROOT) if callable(he) else he)
            args = [os.path.join(progdir, argv[0])] + argv[1:]
        else:
            if argv[0] in aliases:
                prog = '/' + aliases[argv[0]] if side == 'orig' else os.path.relpath(os.path.join(progdir, argv[0]), rd)
            else:
                prog = '/bin/' + argv[0]
            args = [EMU]
            for k, v in senv.items():
                args += ['-e', '%s=%s' % (k, v)]
            args += ['-r', RUNROOT, '-0', argv[0], prog] + argv[1:]	# argv[0] the word, as the shell gives it
            e = env
        args = ['sh', '-c', 'umask 022; exec "$@"', 'sh'] + args	# every step with umask 022 (the modes of new files)
        fin = open(os.path.join(rd, st['stdin']), 'rb') if st['stdin'] else None
        try:
            rc, out, err = spawn(args, rd, e, fin, getattr(cfg, 'TIMEOUT', 600))
        finally:
            if fin:
                fin.close()
        out, err = out.decode('latin-1'), err.decode('latin-1')
        if hasattr(cfg, 'after_step'):
            out, err = cfg.after_step(argv, rc, out, err, rd)
        text = joined(out, err)
        norm = getattr(cfg, 'normalize', None)
        if norm:
            text = norm('output', text)
        res['steps'].append('step %d %s: exit=%d\n%s' % (i, argv[0], rc, text))
    if os.path.exists(rd + '.udcon'):	# what the 8-bit part printed on its console
        res['steps'].append('8-bit console:\n' + open(rd + '.udcon', encoding='latin-1').read())
        os.remove(rd + '.udcon')
    used = collect_rootlog(rd, rd + '.rootlog')
    res['tree'] = listing(rd, getattr(cfg, 'normalize', None))
    res['deps'] = deps_of(RUNROOT, used)
    shutil.rmtree(progdir, ignore_errors=True)
    return res


def check_expect(case, res, rd):
    bad = []
    last = [s for s in res['steps'] if s.startswith('step ')]
    for e in case['expect']:
        if e[0] == 'exit':
            if not last or not last[-1].split('\n', 1)[0].endswith('exit=%s' % e[1]):
                bad.append('exit %s' % e[1])
        elif e[0] == 'output' and e[1] == 'is':	# the whole output of the last step; \n is a newline
            want = e[2].encode('latin-1').decode('unicode_escape')
            if not last or last[-1].split('\n', 1)[1] != want:
                bad.append('output is %s' % e[2])
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
                if case.get('_dir') and e[3].startswith('in/'):	# a starting file of a hand-written case
                    ref = os.path.join(case['_dir'], e[3])
                else:	# another file of the directory after the run
                    ref = os.path.join(rd, e[3])
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


_VALID = set()	# prepared inputs checked in this run


class Generated:
    """a derived case: its name, the source files it is made from, the preparation prep(src, work) -> base or None (made with
    the original tools; None: the original cannot make it, the case is skipped), the text of the case"""

    def __init__(self, cfg, name, src, sources, prep, text, prep_files=()):
        self.cfg, self.name, self.src, self.sources, self.prep, self.text = cfg, name, src, sources, prep, text
        self.prep_files = prep_files
        self._key = None

    def key(self):
        if self._key is None:
            pr = self.prep
            if pr is None:
                pn = 'none'
            elif hasattr(pr, 'func'):	# functools.partial: the function and its arguments (no addresses)
                pn = '%s %r %r' % (pr.func.__name__, pr.args, sorted(pr.keywords.items()))
            else:
                pn = pr.__name__
            parts = ['in', pn, 'src ' + (os.path.basename(self.src) if self.src else '-'), sha1file(EMU)]	# which file of the directory the case is made from
            parts += [os.path.basename(f) + ' ' + sha1file(f) for f in sorted(self.sources)]
            parts += [f + ' ' + sha1tree(os.path.join(ROOT, f)) for f in self.prep_files if os.path.exists(os.path.join(ROOT, f))]
            self._key = mix(*parts)
        return self._key

    def indir(self):
        """the prepared starting files in the store (made when missing), or None when the original cannot make them"""
        k = self.key()
        d = os.path.join(STORE, 'inputs', k[:2], k)
        r = self._valid(d)
        if r is not False:
            return r
        with KeyLock('in' + k):
            r = self._valid(d)
            if r is not False:
                return r
            for old in (d, d + '.skip'):	# made from files of the tree that have changed since (rare: the tree is fixed)
                if os.path.isdir(old):
                    gone = '%s.old%d.%d' % (old, os.getpid(), threading.get_ident())
                    os.rename(old, gone)
                    shutil.rmtree(gone, ignore_errors=True)
                elif os.path.exists(old):
                    os.remove(old)
            tmp = '%s.tmp%d.%d' % (d, os.getpid(), threading.get_ident())
            shutil.rmtree(tmp, ignore_errors=True)
            os.makedirs(tmp)
            r = self.prep(self.src, tmp) if self.prep else True
            deps = json.dumps(deps_of(ROOT, collect_rootlog(tmp)))
            if r is None:
                shutil.rmtree(tmp, ignore_errors=True)
                with open(d + '.skip', 'w') as f:
                    f.write(deps)
                return None
            with open(d + '.deps.tmp', 'w') as f:
                f.write(deps)
            os.replace(d + '.deps.tmp', d + '.deps')
            os.replace(tmp, d)
            return d

    def _valid(self, d):
        """d when the prepared input is there and its files of the tree are unchanged, None for a valid skip, else False"""
        for p, val in ((d, d), (d + '.skip', None)):
            if os.path.exists(p):
                if p in _VALID:
                    return val
                dp = d + '.deps' if val else p
                try:
                    deps = json.loads(open(dp).read() or '{}')
                except (OSError, ValueError):
                    return False
                if deps_ok(ROOT, deps):
                    _VALID.add(p)
                    return val
                return False
        return False


class Gen:
    """what a generator of corpus.py gets: the sources of the tree and how a case is made of them"""

    def __init__(self, cfg):
        self.cfg = cfg

    def c_sources(self):
        import corpusc
        return corpusc.c_sources()

    def dir_files(self, src):
        """the files git knows in the directory of src (what corpusc.copy_dir copies)"""
        import corpusc
        d = os.path.dirname(src)
        names = corpusc.dir_names(d)
        names = os.listdir(d) if names is None else sorted(names)
        return [os.path.join(d, f) for f in names if os.path.isfile(os.path.join(d, f))]

    def name(self, src):
        return os.path.relpath(src, REPO)

    def case(self, src, prep, text, prep_files=(), name=None, sources=None):
        """a case made from the file src of the tree (with the files of its directory), or with src None from nothing
        (an empty directory; name needed); sources: the files the preparation reads, when not those of src's directory
        (cases with the same preparation and sources share one prepared directory)"""
        if sources is None:
            sources = self.dir_files(src) if src else []
        return Generated(self.cfg, name or self.name(src), src, sources, prep, text, prep_files)


def runner_key():
    return mix(sha1file(os.path.abspath(__file__)))


def main():
    a = sys.argv[1:]
    cands_only = None
    keep = nocache = show = False
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
        elif o == '--show':
            show = True
        else:
            print(__doc__)
            return 2
    nocache = nocache or bool(os.environ.get('CORPUS_NOCACHE'))
    cfg = load(a[0])
    cands = {}
    for c, p in cfg.CANDIDATES.items():
        if show or (cands_only and c not in cands_only):
            continue
        path = os.environ.get('CORPUS_BIN_' + c.upper()) or os.path.join(REPO, p)
        if not os.path.exists(path):
            print('%s: %s (%s) not built' % (cfg.NAME, path, c))
            return 1
        cands[c] = path
    global RUNROOT
    if getattr(cfg, 'ROOT_ENV', None):
        RUNROOT = os.environ.get(cfg.ROOT_ENV)
        if not RUNROOT or not os.path.isdir(RUNROOT):
            print('%s: the tree of the original (%s) is not set' % (cfg.NAME, cfg.ROOT_ENV))
            return 1
    base = [sha1file(os.path.join(RUNROOT, cfg.ORIG)), sha1file(EMU), runner_key()]
    base += [f + ' ' + (sha1tree(os.path.join(RUNROOT, f)) if os.path.exists(os.path.join(RUNROOT, f)) else 'missing')
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
        indir = os.path.join(cdir, 'in')
        for vi, var in enumerate(case['variants'] or [{'env': {}, 'words': []}]):
            jobs.append((cname, (lambda d=indir: d), case, vi, var, mix(*([ch, json.dumps(var, sort_keys=True)] + base))))
    if hasattr(cfg, 'generate'):
        gen = Gen(cfg)
        for g in cfg.generate(gen):
            if filt and filt not in g.name:
                continue
            try:
                case = parse_case(g.text, '%s (generated)' % g.name)
            except CaseError as e:
                print('CASE %s' % e)
                return 1
            case['_dir'] = None
            pkey = g.key()
            ch = mix(pkey, g.text)
            for vi, var in enumerate(case['variants'] or [{'env': {}, 'words': []}]):
                jobs.append((g.name, g.indir, case, vi, var, mix(*([ch, json.dumps(var, sort_keys=True)] + base))))
    if show:
        for cname, getin, case, vi, var, okey in jobs:
            vt = ' '.join(['%s=%s' % kv for kv in var['env'].items()] + var['words'])
            indir = getin()
            print('=== %s%s' % (cname, ' [%s]' % vt if vt else ''))
            if indir is None:
                print('    skipped: the original tools cannot prepare it')
                continue
            rdir = os.path.join(work, 'show', '%s.%d' % (cname.replace('/', '_'), vi))
            r = run_case(cfg, indir, case, var, 'orig', None, rdir)
            for st in r['steps']:
                print('    ' + st.rstrip('\n').replace('\n', '\n    '))
            print('    files afterwards:\n      ' + (r['tree'].replace('\n', '\n      ') or '(none)'))
            bad = check_expect(case, r, rdir)
            print('    expectations: ' + ('FAILED: ' + '; '.join(bad) if bad else 'all hold (%d)' % len(case['expect'])))
            shutil.rmtree(rdir, ignore_errors=True)
        return 0
    t0 = time.monotonic()
    same = {c: 0 for c in cands}
    bad = []
    skipped = []

    def one(job):
        cname, getin, case, vi, var, okey = job
        vt = ' '.join(['%s=%s' % kv for kv in var['env'].items()] + var['words'])
        label = cname + ('[%s]' % vt if vt else '')
        base = '%s.%d' % (cname.replace('/', '_'), vi)
        # every side runs in a directory of the same name (WORK/NAME/SIDE/CASE.N): a program that writes its working
        # directory or leftover memory holding it (tar) gives the same bytes on every side
        rd = lambda side: os.path.join(work, side, base)
        msgs = []
        def cached():
            r = None if nocache else store_read('orig', okey)
            return r if r is not None and deps_ok(RUNROOT, r.get('deps')) else None
        r1 = cached()
        if r1 is None:
            with KeyLock(okey):
                r1 = cached()
                if r1 is None:
                    indir = getin()
                    if indir is None:
                        return label, None, {}
                    ra = run_case(cfg, indir, case, var, 'orig', None, rd('orig'))
                    rb = run_case(cfg, indir, case, var, 'orig', None, rd('orig2'))
                    if ra != rb:
                        return label, ['UNSTABLE original: ' + explain(ra, rb)[0]], {c: False for c in cands}
                    shutil.rmtree(rd('orig2'), ignore_errors=True)
                    r1 = ra
                    r1['expect'] = check_expect(case, ra, rd('orig'))
                    store_write('orig', okey, r1)
        if r1.get('expect'):
            msgs.append('EXPECT failed on the original: ' + '; '.join(r1['expect']))
        res = {}
        for c, path in cands.items():
            ckey = mix(okey, json.dumps(r1.get('deps', {}), sort_keys=True), sha1file(path),
                       *[sha1tree(os.path.join(REPO, d)) for d in getattr(cfg, 'DEPENDS', {}).get(c, [])])
            ok = None if nocache else store_read('ok', ckey)
            if isinstance(ok, dict) and deps_ok(RUNROOT, ok.get('deps')):
                res[c] = not msgs
                continue
            indir = getin()
            if indir is None:
                return label, None, {}
            r2 = run_case(cfg, indir, case, var, c, path, rd(c))
            r1c = {'steps': r1['steps'], 'tree': r1['tree']}
            if r1c == {'steps': r2['steps'], 'tree': r2['tree']}:
                store_write('ok', ckey, {'deps': r2['deps']})
                res[c] = not msgs
                if not keep:
                    shutil.rmtree(rd(c), ignore_errors=True)
            else:
                res[c] = False
                msgs.append('[%s] %s' % (c, explain(r1c, {'steps': r2['steps'], 'tree': r2['tree']})[0]))
        if not keep:
            shutil.rmtree(rd('orig'), ignore_errors=True)
        return label, msgs, res

    n = int(os.environ.get('JOBS') or max(1, (os.cpu_count() or 2) - 1))
    with concurrent.futures.ThreadPoolExecutor(n) as ex:
        for label, msgs, res in ex.map(one, jobs):
            if msgs is None:
                skipped.append(label)
                continue
            for c, ok in res.items():
                if ok:
                    same[c] += 1
            if msgs:
                bad.append(label)
                print('DIFF %s: %s' % (label, '\n    '.join(msgs)), flush=True)
    for c in cands:
        print('%s [%s]: %d of %d inputs identical (%d sources skipped)' % (cfg.NAME, c, same[c], len(jobs) - len(skipped),
                                                                           len(skipped)), flush=True)
    print('TIME %-8s wall %7.1f   %d cases' % (cfg.NAME, time.monotonic() - t0, len(jobs)))
    return 1 if bad or any(same[c] != len(jobs) - len(skipped) for c in cands) else 0


if __name__ == '__main__':
    sys.exit(main())
