"""corpusc.py - the inputs of the corpus tests of the compiler passes (the CORPUS_* functions of a progcfg.py).

The inputs are the C sources of src/cmd and src/lib (the files known to git; in a copy without git every .c outside the
build, old and tools directories). The input of a pass is made from a source by the original earlier passes:
  prepare_cpp(src, work)            the source and the headers of its directory; returns the source's name (cpp)
  prepare_i(src, work)              t.i, the original /lib/cpp on it (cparse, scparse); returns 't'
  prepare_ids(src, work, cc)        x.I, x.D, x.S, kept by the original `cc -S -:d` (codgen: /bin/cc, scodgen: /bin/scc)
  prepare_s(src, work, cc)          in.s, the original `cc -S` without -O: the input of popt (spopt: /bin/scc)
Each returns None when the original cannot make the input; the case is skipped then.
"""
import os
import shutil
import subprocess

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))     # the WEGA directory of the repository


def root():
    return os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'


def emu():
    return os.environ.get('EMU', os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu'))


def c_sources():
    """the C sources of src/cmd and src/lib, sorted"""
    r = subprocess.run(['git', '-C', REPO, 'ls-files', '--', 'src/cmd', 'src/lib'], capture_output=True, text=True)
    if r.returncode == 0 and r.stdout:
        fs = [os.path.join(REPO, f) for f in r.stdout.split('\n') if f.endswith('.c')]
    else:
        fs = []
        for top in ('src/cmd', 'src/lib'):
            for d, dirs, names in os.walk(os.path.join(REPO, top)):
                dirs[:] = [x for x in dirs if x not in ('build', 'old', 'tools')]
                fs += [os.path.join(d, f) for f in names if f.endswith('.c')]
    return sorted(f for f in fs if '/tools/' not in f and '/shipped/' not in f)


def run(argv, work, timeout=120):
    return subprocess.run([emu(), '-r', root()] + argv, cwd=work, capture_output=True, timeout=timeout)


def copy_dir(src, work):
    """the source and the other files of its directory (headers, included .c files)"""
    d = os.path.dirname(src)
    for f in os.listdir(d):
        p = os.path.join(d, f)
        if os.path.isfile(p):
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


def prepare_s(src, work, cc):
    copy_dir(src, work)
    b = os.path.basename(src)[:-2]
    try:
        r = run([cc, '-S', '-I.', b + '.c'], work, 60)
    except subprocess.TimeoutExpired:
        return None
    if r.returncode != 0 or not os.path.exists(os.path.join(work, b + '.s')):
        return None
    os.rename(os.path.join(work, b + '.s'), os.path.join(work, 'in.s'))
    return 'in'
