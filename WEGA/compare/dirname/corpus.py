"""Corpus of dirname (host/recon/corpusrun.py): the original script /usr/bin/dirname against src/cmd/dirname.sh, at its place in the tree
(INSTALLED), on the cases of compare/dirname/tests.txt. dirname prints the path without its last component (expr); it is the same text as basename, the name of the call ($0) decides."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'dirname'
ORIG = 'usr/bin/dirname'
PROG = 'dirname'
CANDIDATES = {'wega': 'src/cmd/dirname.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/dirname', prefix=PRE)
