"""Corpus of pstat (host/recon/corpusrun.py): the original /bin/pstat against the build. The cases are the blocks of
compare/pstat/tests.txt (host/recon/casefile.py: the format; the default step is pstat). Rudimentary: the emulator has
no /dev/kmem (increment 162), so the cases read the crash dump of the corpus of ps (compare/ps/mkdump.py, through
kva.c): the usage, a core that cannot be opened, a namelist that cannot be read, the process table (-p, -ap). Every case has an
expectation on the original, written from the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'pstat'
ORIG = 'bin/pstat'
PROG = 'pstat'
CANDIDATES = {'wega': 'src/cmd/pstat/pstat'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'pstat')
