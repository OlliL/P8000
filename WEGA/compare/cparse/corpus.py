"""Corpus of cparse (host/recon/corpusrun.py): every C source of src/cmd and src/lib and the cases of cases/, made into t.i by
the original cpp, then `cparse -E t` < t.i (t.I, t.D, t.S are compared); the original against the shipped build and the host
build. scparse (compare/scparse/corpus.py) is the same with the segmented pass. The settings of the reconstruction tools stay
in progcfg.py."""
import glob
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'cparse'
ORIG = 'lib/cparse'
PROG = 'cparse'
CANDIDATES = {'wega': 'src/cmd/cparse/build/nonseg/cparse', 'host': 'host/cparse/cparse'}
ROOT_FILES = ['usr/include']
PREP_FILES = ['lib/cpp', 'usr/include']
EXPECT = {'src/lib/libc/common/gen/abs.c': 'expect: exit 0\nexpect: file t.I contains abs\n'}


def normalize(what, data):
    """the .S file without the register byte (+5) of the symbols that are no register variable and no field: cparse sets
    it only for these, else the byte is what the heap held before (KNOWLEDGE.md, quirks of /lib/cparse); it differs in a
    build with another data layout"""
    if isinstance(data, str) or not what.endswith('.S'):
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


def sources(g):
    cases = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'cparse', 'cases')
    fixed = sorted(glob.glob(os.path.join(cases, '*.c'))) + [os.path.join(cases, 'yacc', 'cpy.c')]
    return g.c_sources() + [os.path.normpath(f) for f in fixed]


def generate(g):
    for src in sources(g):
        yield g.case(src, corpusc.prepare_i, 'step: %s -E t\nstdin: t.i\n' % PROG + EXPECT.get(g.name(src), ''),
                     prep_files=PREP_FILES)
