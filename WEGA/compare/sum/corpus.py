"""Corpus of sum (host/recon/corpusrun.py): the original /bin/sum against the shipped build, on the cases of
compare/sum/tests.txt (host/recon/casefile.py): the man page (sum.1) and every message, option and path of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'sum'
ORIG = 'bin/sum'
PROG = 'sum'
CANDIDATES = {'wega': 'src/cmd/sum/sum'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'sum t.in')
