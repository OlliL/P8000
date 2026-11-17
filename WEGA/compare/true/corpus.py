"""Corpus of true (host/recon/corpusrun.py): the original script /usr/bin/true against src/cmd/true.sh, at its place in the tree
(INSTALLED), on the cases of compare/true/tests.txt. true does nothing and ends with status 0."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'true'
ORIG = 'usr/bin/true'
PROG = 'true'
CANDIDATES = {'wega': 'src/cmd/true.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/true', prefix=PRE)
