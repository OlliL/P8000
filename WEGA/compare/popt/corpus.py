"""Corpus of popt (host/recon/corpusrun.py): every C source of src/cmd and src/lib and the cases of compare/popt/cases, made into
in.s by the original cc -S, then `popt [-l|-d] in.s out.s`; the original against the shipped build and the host build. The
settings of the reconstruction tools stay in progcfg.py."""
import functools
import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'popt'
ORIG = 'lib/popt'
PROG = 'popt'
CANDIDATES = {'wega': 'src/cmd/popt/build/nonseg/popt', 'host': 'host/popt/popt'}
PREP_FILES = ['bin/cc', 'lib/cpp', 'lib/cparse', 'lib/codgen', 'usr/include']
EXPECT = {'src/lib/libc/common/gen/abs.c': 'expect: exit 0\nexpect: file out.s contains _abs\n'}


def normalize(what, data):
    """the size of the core popt reports may differ (`NNK core`)"""
    if isinstance(data, str):
        return re.sub(r'\d+K core', 'K core', data)
    return data


def generate(g):
    prep = functools.partial(corpusc.prepare_s, cc='/bin/cc')
    cases = sorted(glob.glob(os.path.join(HERE, '..', 'popt', 'cases', '*.c')))
    for src in g.c_sources() + [os.path.normpath(f) for f in cases]:
        text = 'step: popt $V in.s out.s\nvariant:\nvariant: -l\nvariant: -d\n' + EXPECT.get(g.name(src), '')
        yield g.case(src, prep, text, prep_files=PREP_FILES)
