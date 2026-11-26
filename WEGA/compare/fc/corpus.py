"""Corpus of fc (host/recon/corpusrun.py): the original /bin/fc against the build. The cases are the blocks of
compare/fc/tests.txt (host/recon/casefile.py: the format): rudimentary, the paths of src/cmd/fse/fc.c that do not start
fse: the usage, a compilation without errors (/bin/cc) and one whose errors name no file (a file that cannot be read), which are
printed. Every case has an expectation on the original, written from the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'fc'
ORIG = 'bin/fc'
PROG = 'fc'
CANDIDATES = {'wega': 'src/cmd/fse/fc'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'fc t.c', first='t.c')
