"""corpusc.py - the inputs of the corpus tests of the compiler passes (the CORPUS_* functions of a progcfg.py).

The inputs are the C sources of src/cmd and src/lib (the files known to git; in a copy without git every .c outside the
build, old and tools directories). The input of a pass is made from a source by the original earlier passes:
  prepare_cpp(src, work)            the source and the headers of its directory; returns the source's name (cpp)
  prepare_i(src, work)              t.i, the original /lib/cpp on it (cparse, scparse); returns 't'
  prepare_ids(src, work, cc)        x.I, x.D, x.S, kept by the original `cc -S -:d` (codgen: /bin/cc, scodgen: /bin/scc)
  prepare_s(src, work, cc[, opts])  in.s, the original `cc -S` (without -O: the input of popt, spopt: /bin/scc; cas: -O)
Each returns None when the original cannot make the input; the case is skipped then.
"""
import os
import shutil
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))     # the WEGA directory of the repository


def root():
    return os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/home/olivleh1/git/P8000/tmp/wega31_root'


def emu():
    return os.environ.get('EMU', os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu'))


def tree_files(d):
    """the files of the tree below the directory d (absolute), relative to REPO: the list .files of a checkall snapshot, or
    what git knows (tracked and untracked, not ignored); None without either"""
    rel = os.path.relpath(d, REPO)
    if rel.startswith('..'):
        return None
    pre = '' if rel == '.' else rel + '/'
    lst = os.path.join(REPO, '.files')
    if os.path.exists(lst):
        return [f for f in open(lst).read().split('\n') if f and f.startswith(pre)]
    r = subprocess.run(['git', '-C', REPO, 'ls-files', '-co', '--exclude-standard', '--', rel], capture_output=True, text=True)
    if r.returncode == 0 and r.stdout:
        return [f for f in r.stdout.split('\n') if f]
    return None


def c_sources():
    """the C sources of src/cmd and src/lib, sorted"""
    fs = None
    if tree_files(os.path.join(REPO, 'src')) is not None:
        fs = [os.path.join(REPO, f) for t in ('src/cmd', 'src/lib') for f in tree_files(os.path.join(REPO, t)) or [] if f.endswith('.c')]
    if fs is None:
        fs = []
        for top in ('src/cmd', 'src/lib'):
            for d, dirs, names in os.walk(os.path.join(REPO, top)):
                dirs[:] = [x for x in dirs if x not in ('build', 'old')]
                fs += [os.path.join(d, f) for f in names if f.endswith('.c')]
    rel = lambda f: '/' + os.path.relpath(f, REPO)
    return sorted(f for f in fs if '/compare/' not in rel(f) and '/shipped/' not in rel(f))


def run(argv, work, timeout=120):
    return subprocess.run([emu(), '-r', root()] + argv, cwd=work, capture_output=True, timeout=timeout)


_GITFILES = {}


def dir_names(d):
    """the names of the files of the tree directly in the directory d (tree_files), None when unknown"""
    d = os.path.abspath(d)
    if d not in _GITFILES:
        fs = tree_files(d)
        n = len(os.path.relpath(d, REPO)) + 1
        _GITFILES[d] = None if fs is None else set(f[n:] for f in fs if '/' not in f[n:])
    return _GITFILES[d]


def copy_dir(src, work):
    """the source and the other files of its directory (headers, included .c files): the files git knows there (tracked or
    not ignored, as the copy of checkall.sh), not what a build has left (objects, programs: they would make the inputs depend
    on the state of the tree when the tar was made)"""
    d = os.path.dirname(src)
    keep = dir_names(d)
    for f in os.listdir(d):
        p = os.path.join(d, f)
        if os.path.isfile(p) and (keep is None or f in keep):
            shutil.copy(p, work)


def prepare_cpp(src, work):
    copy_dir(src, work)
    return os.path.basename(src)


def prepare_i(src, work):
    copy_dir(src, work)
    r = run(['/lib/cpp', '-I.', os.path.basename(src), 't.i'], work)
    if r.returncode != 0 or not os.path.exists(os.path.join(work, 't.i')):
        return None
    return 't'


def prepare_ids(src, work, cc):
    copy_dir(src, work)
    try:
        r = run([cc, '-S', '-:d', '-I.', os.path.basename(src)], work, 60)
    except subprocess.TimeoutExpired:
        return None
    base = [f[:-2] for f in os.listdir(work) if f.startswith('cc.') and f.endswith('.I')]
    if r.returncode != 0 or len(base) != 1:
        return None
    for ext in ('I', 'D', 'S'):
        p = os.path.join(work, base[0] + '.' + ext)
        if not os.path.exists(p):
            return None
        os.rename(p, os.path.join(work, 'x.' + ext))
    return 'x'


def prepare_s(src, work, cc, opts=()):
    copy_dir(src, work)
    b = os.path.basename(src)[:-2]
    try:
        r = run([cc, '-S'] + list(opts) + ['-I.', b + '.c'], work, 60)
    except subprocess.TimeoutExpired:
        return None
    if r.returncode != 0 or not os.path.exists(os.path.join(work, b + '.s')):
        return None
    os.rename(os.path.join(work, b + '.s'), os.path.join(work, 'in.s'))
    return 'in'
