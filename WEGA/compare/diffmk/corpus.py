"""Corpus of diffmk (host/recon/corpusrun.py): the original script /usr/bin/diffmk against src/cmd/diffmk.sh, at its place in the tree
(INSTALLED), on the cases of compare/diffmk/tests.txt. diffmk marks the changes between two versions of a text with .mc requests of nroff (diff -e, sed, ed)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'diffmk'
ORIG = 'usr/bin/diffmk'
PROG = 'diffmk'
CANDIDATES = {'wega': 'src/cmd/diffmk.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/diffmk', prefix=PRE)
