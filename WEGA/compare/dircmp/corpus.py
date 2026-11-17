"""Corpus of dircmp (host/recon/corpusrun.py): the original script /usr/bin/dircmp against src/cmd/dircmp.sh, at its place in the tree
(INSTALLED), on the cases of compare/dircmp/tests.txt. dircmp compares two directories (find, comm, cmp, diff, pr; pr heads the pages with the clock, fixed here)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'dircmp'
ORIG = 'usr/bin/dircmp'
PROG = 'dircmp'
CANDIDATES = {'wega': 'src/cmd/dircmp.sh'}
INSTALLED = True
IN_TMP = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/dircmp', prefix=PRE)
