"""Configuration of the recon toolkit (host/recon/progcfg.py) for cparse (/lib/cparse: nonsegmented E711, stripped).

cparse is pass 1 of cc (PCC front end): it reads the cpp output from stdin and writes base.I (intermediate text for
codgen), base.D (data and string literals as cas text) and base.S (symbol table) for `cparse -E base`.
"""
import os

NAME = 'cparse'
ORIG = 'lib/cparse'
OWN_LIM = 0x9f06    # the own code ends here; the libc part follows
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|cgram1|xdefs1|scan1|pftn1|trees1|optim1|code1|local1|comm1|vinit1|sym1|sym2)'
CC = '/bin/cc'
# modules in the order of their SCCS stamps in the data of /lib/cparse (optim and common are files included by
# optim1.c and comm1.c, guess from PCC; cgram1.y is made into cgram1.c by yacc)
MODULES = ['cgram1', 'xdefs1', 'scan1', 'pftn1', 'trees1', 'optim1', 'code1', 'local1', 'comm1', 'vinit1', 'sym1', 'sym2']
TABLES = []

ROOTDIR = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
REPO = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


CORPUS_BINARY = 'build/nonseg/cparse'
CANDIDATES = {'wega': 'src/cmd/cparse/build/nonseg/cparse', 'host': 'host/cparse/cparse'}


def CORPUS_SOURCES():
    import corpusc
    # the sources of the tree and the fixed cases (compare/cparse/cases: a structure declared in a function, whose tag is freed at its end)
    cases = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'cparse', 'cases')
    fixed = sorted(os.path.join(cases, f) for f in os.listdir(cases) if f.endswith('.c'))
    fixed.append(os.path.join(cases, 'yacc', 'cpy.c'))	# with yylex.c next to it, which it includes
    return corpusc.c_sources() + fixed


def CORPUS_PREPARE(src, work):
    """the original cpp makes the input of cparse (t.i)"""
    import corpusc
    return corpusc.prepare_i(src, work)


def CORPUS_NORMALIZE(f, data):
    """the .S file without the register byte (+5) of the symbols that are no register variable and no field: cparse sets
    it only for these, else the byte is what the heap held before (KNOWLEDGE.md, quirks of /lib/cparse); it differs in a
    build with another data layout"""
    import struct
    if not f.endswith('.S'):
        return data
    d = bytearray(data)
    pos = 4
    while pos + 14 <= len(d):
        ns, na = struct.unpack('>II', bytes(d[pos + 2:pos + 10]))
        nn, nm = struct.unpack('>HH', bytes(d[pos + 10:pos + 14]))
        for k in range(pos + 14, pos + 14 + ns, 22):
            if k + 5 < len(d) and d[k + 4] != 4 and d[k + 4] < 0x40:
                d[k + 5] = 0
        pos += 14 + ns + na + nn + nm
    return bytes(d)


def CORPUS_RUN(binary, base, work):
    return [binary, '-E', base], base + '.i'


def CORPUS_OUTPUTS(base):
    return [base + '.I', base + '.D', base + '.S']

# D18: the cc that built /lib/cparse never writes @r15 (506 times %0000(r15), 0 times @r15; ld and codgen use @r15);
# the comparisons treat the two forms as equal
D18 = True


def model_link(work, objs):
    """the link of d12model.py: crt0.o (make crt0.o), our modules, the libc of the program, like `make cparse`"""
    import os
    here = os.path.dirname(os.path.abspath(__file__))
    cp = os.path.normpath(os.path.join(here, '..', '..', 'src', 'cmd', 'cparse'))
    crt = os.path.join(cp, 'crt0.o')
    hostld = os.path.normpath(os.path.join(cp, '..', '..', '..', 'host', 'ld', 'ld'))
    libc = [os.path.join(cp, '..', '..', 'lib', 'libc-v7.a')]
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', crt] + objs + libc + ['-o', 'cparsem']


# the strict comparison runs on the d12model link (our data layout differs from the original only by D12)
import os as _os  # noqa: E402
MODEL_OUT = _os.path.join(_os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cparse', 'cparsem')
STRICT_INPUT = MODEL_OUT

# D6: the switch tables of the model in the layout of the old cc (host/recon/d12model.py d6tables)
D6TABLES = True
