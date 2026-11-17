"""Corpus of pr (host/recon/corpusrun.py): the original /bin/pr against the shipped build, on the cases of
compare/pr/tests.txt (host/recon/casefile.py): the man page (pr.1) and every message, option and path of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'pr'
ORIG = 'bin/pr'
PROG = 'pr'
CANDIDATES = {'wega': 'src/cmd/pr'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'pr t.in')
