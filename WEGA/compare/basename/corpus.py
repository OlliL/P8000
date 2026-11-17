"""Corpus of basename (host/recon/corpusrun.py): the original script /usr/bin/basename against src/cmd/basename.sh, at its place in the tree
(INSTALLED), on the cases of compare/basename/tests.txt. basename prints the last component of a path name (expr)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'basename'
ORIG = 'usr/bin/basename'
PROG = 'basename'
CANDIDATES = {'wega': 'src/cmd/basename.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/basename', prefix=PRE)
