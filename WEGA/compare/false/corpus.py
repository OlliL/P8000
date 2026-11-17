"""Corpus of false (host/recon/corpusrun.py): the original script /usr/bin/false against src/cmd/false.sh, at its place in the tree
(INSTALLED), on the cases of compare/false/tests.txt. false does nothing and ends with status 255."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'false'
ORIG = 'usr/bin/false'
PROG = 'false'
CANDIDATES = {'wega': 'src/cmd/false.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/false', prefix=PRE)
