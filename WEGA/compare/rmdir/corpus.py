"""Corpus of rmdir (host/recon/corpusrun.py): the original /bin/rmdir against the shipped build, on the cases of
compare/rmdir/tests.txt (host/recon/casefile.py): the man page (rm.1) and every message of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'rmdir'
ORIG = 'bin/rmdir'
PROG = 'rmdir'
CANDIDATES = {'wega': 'src/cmd/rmdir'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'rmdir t.in')
