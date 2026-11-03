"""Configuration of the recon toolkit (host/recon/progcfg.py) for cpp (/lib/cpp), built with ccv0p."""
import os

NAME = 'cpp'
ORIG = 'lib/cpp'
OWN_LIM = 0x2272    # the own code ends here (the last ret of yyparse); fopen of the libc follows
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|cpp|cpy)'
MODULES = ['cpp', 'cpy']
TABLES = []
D18 = True
COMBINED = True    # code and data together (strictcmp.py)
D6TABLES = True
STATICFUNCS = True


def model_link(work, objs):
    """the link of d12model.py: crt0.o (make crt0.o), our modules, the libc part of the Makefile"""
    import re
    here = os.path.dirname(os.path.abspath(__file__))
    src = os.path.normpath(os.path.join(here, '..'))
    hostld = os.path.normpath(os.path.join(src, '..', '..', '..', 'host', 'ld', 'ld'))
    mk = open(os.path.join(src, 'Makefile')).read().replace('\\\n', ' ')
    libc = re.search(r'^LIBCOBJS = (.*)$', mk, re.M).group(1).split()
    libc = [os.path.join(src, o.replace('$(OLD)', os.path.join('..', '..', 'lib', 'oldlibc'))) for o in libc]
    return [os.environ.get('LDBIN', hostld), '-e', 'start', os.path.join(src, 'crt0.o')] + objs + libc + ['-o', 'cppm']


MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cpp', 'cppm')
STRICT_INPUT = MODEL_OUT

REPO_ = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))


def CORPUS_SOURCES():
    """every C source of src/cmd and src/lib, sorted"""
    out = []
    for top in ('src/cmd', 'src/lib'):
        for d, _, fs in os.walk(os.path.join(REPO_, top)):
            if '/tools' in d or d.endswith('/shipped'):
                continue
            out += [os.path.join(d, f) for f in fs if f.endswith('.c')]
    return sorted(out)


def CORPUS_PREPARE(src, work):
    """the source and the headers of its directory in work"""
    import shutil
    d = os.path.dirname(src)
    for f in os.listdir(d):
        p = os.path.join(d, f)
        if os.path.isfile(p) and (f.endswith('.h') or f == os.path.basename(src)):
            shutil.copy(p, work)
    return os.path.basename(src)


def CORPUS_RUN(binary, base, work):
    return [binary, '-I.', base, 't.i'], None


def CORPUS_OUTPUTS(base):
    return ['t.i']
