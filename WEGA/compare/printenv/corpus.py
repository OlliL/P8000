"""Corpus of printenv (host/recon/corpusrun.py): the original /bin/printenv against the build (src/cmd/printenv.c of 2BSD,
ccv0, libc-v1.a), on the cases of compare/printenv/tests.txt (host/recon/casefile.py): all, one name, a missing name, a prefix."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'printenv'
ORIG = 'bin/printenv'
PROG = 'printenv'
CANDIDATES = {'wega': 'src/cmd/printenv'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'printenv')
