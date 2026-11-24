"""Corpus of diff (host/recon/corpusrun.py): the original /bin/diff against the build (src/cmd/diff.c, the shipped cc and libc,
split I/D), on the cases of compare/diff/tests.txt (host/recon/casefile.py): plain, -e, -f, -b, -h, equal and missing files, stdin."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'diff'
ORIG = 'bin/diff'
PROG = 'diff'
CANDIDATES = {'wega': 'src/cmd/diff'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'diff')
