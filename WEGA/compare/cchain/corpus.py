"""Corpus of the C compiler chain (host/recon/corpusrun.py): the driver cc compiles every C source of src/cmd and src/lib
(-c, without and with -O) and links the small programs of link/ (three steps: -c, the link, -s from the source); the original
/bin/cc with the original passes under the emulator against the host driver with the host passes (its cc.conf, written next
to it). scchain (compare/scchain/corpus.py) is the same with scc and the segmented passes."""
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.normpath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(REPO, 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'cchain'
ORIG = 'bin/cc'
PROG = 'cc'
CONFNAME = 'cc.conf'
# the passes of the chain: the host binary (relative to the WEGA directory) and the original in the tree
PASSES = {'cpp': ('host/cc/cpp/cpp', 'lib/cpp'), 'cparse': ('host/cc/cparse/cparse', 'lib/cparse'), 'codgen': ('host/cc/codgen/codgen', 'lib/codgen'),
          'popt': ('host/cc/popt/popt', 'lib/popt'), 'cas': ('host/cas/cas', 'bin/cas'), 'ld': ('host/ld/ld', 'bin/ld')}
CRT0, LIBC = 'lib/crt0.o', 'lib/libc.a'
CANDIDATES = {'host': 'host/cc/cc'}


def derived(passes, crt0, libc):
    """DEPENDS, ROOT_FILES that follow from the passes and the start-up file and library (also for scchain)"""
    return {'host': [v[0] for v in passes.values()]}, ['usr/include', 'bin/mv', crt0, libc] + [v[1] for v in passes.values()]


DEPENDS, ROOT_FILES = derived(PASSES, CRT0, LIBC)


def HOST_ENV(root):
    return {'CPP_INCLUDE': os.path.join(root, 'usr', 'include')}


def make_host_setup(passes, crt0, libc, confname):
    def host_setup(progdir):
        """the cc.conf of the host driver, next to it: the host passes and the files of the tree"""
        root = corpusc.root()
        conf = ''.join('%s = %s\n' % (k, os.path.join(REPO, v[0])) for k, v in passes.items())
        conf += 'crt0 = %s\nlibc = %s\ntmpdir = .\n' % (os.path.join(root, crt0), os.path.join(root, libc))
        open(os.path.join(progdir, confname), 'w').write(conf)
    return host_setup


host_setup = make_host_setup(PASSES, CRT0, LIBC, CONFNAME)


def normalize(what, data):
    """the process id in the names of the temporary files (cc.<pid>.<x>); the include directory of the host cpp"""
    if not isinstance(data, str):
        return data
    data = data.replace(os.path.join(corpusc.root(), 'usr', 'include'), '/usr/include')
    return re.sub(r'cc\.-?[0-9]+', 'cc.PID', data)


def prepare_link(src, work):
    shutil.copy(src, os.path.join(work, 'lk_' + os.path.basename(src)))
    return 'lk'


def generate_for(g, prog):
    for src in g.c_sources():
        b = os.path.basename(src)
        yield g.case(src, corpusc.prepare_cpp, 'step: %s -c $V -I. %s\nvariant:\nvariant: -O\n' % (prog, b))
    for f in sorted(os.listdir(os.path.join(HERE, 'link'))):
        if f.endswith('.c'):
            n = 'lk_' + f[:-2]
            text = ('step: {p} -c $V {n}.c\nstep: {p} $V -o {n}.out {n}.o\nstep: {p} -s $V -o {n}.s.out {n}.c\n'
                    'variant:\nvariant: -O\n').format(p=prog, n=n)
            if f == 'hello.c':
                text += 'expect: file lk_hello.out contains hello\n'
            yield g.case(os.path.join(HERE, 'link', f), prepare_link, text, name='link/' + f)


def generate(g):
    return generate_for(g, PROG)
