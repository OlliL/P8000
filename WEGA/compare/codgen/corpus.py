"""Corpus of codgen (host/recon/corpusrun.py): every C source of src/cmd and src/lib, made into x.I, x.D, x.S by the original
cc (`-S -:d`), then `codgen -E x` < x.I (its assembler output is compared); the original against the shipped build and the
host build. The settings of the reconstruction tools stay in progcfg.py."""
import functools
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'codgen'
ORIG = 'lib/codgen'
PROG = 'codgen'
CANDIDATES = {'wega': 'src/cmd/cc/codgen/build/nonseg/codgen', 'host': 'host/codgen/codgen'}
PREP_FILES = ['bin/cc', 'lib/cpp', 'lib/cparse', 'usr/include']
EXPECT = {'src/lib/libc/common/gen/abs.c': 'expect: exit 0\nexpect: output contains _abs\n'}


def generate(g):
    prep = functools.partial(corpusc.prepare_ids, cc='/bin/cc')
    for src in g.c_sources():
        yield g.case(src, prep, 'step: codgen -E x\nstdin: x.I\n' + EXPECT.get(g.name(src), ''), prep_files=PREP_FILES)
