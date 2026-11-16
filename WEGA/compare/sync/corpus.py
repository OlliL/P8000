"""Corpus of sync (host/recon/corpusrun.py): the original /bin/sync against the shipped build, on the cases of
compare/sync/tests.txt (host/recon/casefile.py): the man page (manM/sync.M) and src/cmd/sync.c calls sync() and exits with 0; arguments, the standard
input and the user do not matter."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'sync'
ORIG = 'bin/sync'
PROG = 'sync'
CANDIDATES = {'wega': 'src/cmd/sync'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'sync')
