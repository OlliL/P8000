"""Configuration of the recon toolkit (host/recon/progcfg.py) for cparse (/lib/cparse: nonsegmented E711, stripped).

cparse is pass 1 of cc (PCC front end): it reads the cpp output from stdin and writes base.I (intermediate text for
codgen), base.D (data and string literals as cas text) and base.S (symbol table) for `cparse -E base`.
"""
import os

NAME = 'cparse'
ORIG = 'lib/cparse'
OWN_LIM = 0x9f06    # the own code ends here; the libc part follows (HANDOFF_cparse.md, step 2)
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|cgram1|xdefs1|scan1|pftn1|trees1|optim1|code1|local1|comm1|vinit1|sym1)'
CC = '/bin/cc'
# modules in the order of their SCCS stamps in the data of /lib/cparse (optim and common are files included by
# optim1.c and comm1.c, guess from PCC; cgram1.y is made into cgram1.c by yacc)
MODULES = ['bss', 'cgram1', 'xdefs1', 'scan1', 'pftn1', 'trees1', 'optim1', 'code1', 'local1', 'comm1', 'vinit1', 'sym1']
TABLES = []

ROOTDIR = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))


def CORPUS_SOURCES():
    """every C source of src/cmd and src/lib (the corpus of codgen), sorted"""
    out = []
    for top in ('src/cmd', 'src/lib'):
        for d, _, fs in os.walk(os.path.join(REPO, top)):
            if '/tools' in d or d.endswith('/shipped'):
                continue
            out += [os.path.join(d, f) for f in fs if f.endswith('.c')]
    return sorted(out)


def CORPUS_PREPARE(src, work):
    """the original cpp makes the input of cparse (t.i) in work; the directory of the source is the include path"""
    import shutil
    import subprocess
    emu = os.environ.get('EMU', os.path.join(REPO, 'host', 'z8k-emu', 'z8k-emu'))
    d = os.path.dirname(src)
    for f in os.listdir(d):
        p = os.path.join(d, f)
        if os.path.isfile(p) and (f.endswith('.h') or f == os.path.basename(src)):
            shutil.copy(p, work)
    r = subprocess.run([emu, '-r', ROOTDIR, '/lib/cpp', '-I.', os.path.basename(src), 't.i'], cwd=work,
                       capture_output=True, timeout=120)
    if r.returncode != 0 or not os.path.exists(os.path.join(work, 't.i')):
        return None
    return 't'


def CORPUS_RUN(binary, base, work):
    return [binary, '-E', base], base + '.i'


def CORPUS_OUTPUTS(base):
    return [base + '.I', base + '.D', base + '.S']

# D18: the cc that built /lib/cparse never writes @r15 (506 times %0000(r15), 0 times @r15; ld and codgen use @r15);
# the comparisons treat the two forms as equal
D18 = True


def model_link(work, objs):
    """the link of d12model.py: crt0.o (make crt0.o), our modules, the libc part, like `make cparse`"""
    import os
    here = os.path.dirname(os.path.abspath(__file__))
    cp = os.path.normpath(os.path.join(here, '..'))
    crt = os.path.join(cp, 'crt0.o')
    hostld = os.path.normpath(os.path.join(cp, '..', '..', '..', 'host', 'ld', 'ld'))
    import re
    mk = open(os.path.join(cp, 'Makefile')).read().replace('\\\n', ' ')
    libc = re.search(r'^LIBCOBJS = (.*)$', mk, re.M).group(1).split()
    libc = [os.path.join(cp, o.replace('$(OLD)', os.path.join('..', '..', 'lib', 'oldlibc'))) for o in libc]
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', crt] + objs + libc + ['-o', 'cparsem']


# the strict comparison runs on the d12model link (our data layout differs from the original only by D12)
import os as _os  # noqa: E402
MODEL_OUT = _os.path.join(_os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cparse', 'cparsem')
STRICT_INPUT = MODEL_OUT

# D6: the switch tables of the model in the layout of the old cc (host/recon/d12model.py d6tables)
D6TABLES = True
