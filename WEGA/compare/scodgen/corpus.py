"""Corpus of scodgen (host/recon/corpusrun.py): every C source of src/cmd and src/lib, made into x.I, x.D, x.S by the original
scc (`-S -:d`), then `scodgen -E x` < x.I (its assembler output is compared); the original against the shipped build and the
host build. The settings of the reconstruction tools stay in progcfg.py."""
import functools
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'scodgen'
ORIG = 'lib/scodgen'
PROG = 'scodgen'
CANDIDATES = {'wega': 'src/cmd/cc/codgen/build/seg/scodgen', 'host': 'host/cc/codgen/scodgen'}
PREP_FILES = ['bin/scc', 'lib/cpp', 'lib/scparse', 'usr/include']
EXPECT = {'src/lib/libc/common/gen/abs.c': 'expect: exit 0\nexpect: output contains _abs\n'}


def generate(g):
    prep = functools.partial(corpusc.prepare_ids, cc='/bin/scc')
    for src in g.c_sources():
        yield g.case(src, prep, 'step: scodgen -E x\nstdin: x.I\n' + EXPECT.get(g.name(src), ''), prep_files=PREP_FILES)
