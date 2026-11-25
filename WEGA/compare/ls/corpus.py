"""Corpus of ls (host/recon/corpusrun.py): the original /bin/ls against the build. The cases are the blocks of
compare/ls/tests.txt (host/recon/casefile.py: the format; the default step is ls): the options of the usage line
(-1ACDFRacdfgilmnqrstux), the names of the program (a name ending in l, x, f picks the format on a terminal), the long
listing and its columns, the sorting, -R, the diagnostics. Every case has an expectation on the original, written from
the man page or the source.
The times of the long listing are those of the files of the case: the steps set them by touch (time: of the case) so that
both runs print the same dates."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'ls'
ORIG = 'bin/ls'
PROG = 'ls'
CANDIDATES = {'wega': 'src/cmd/ls'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'ls')
