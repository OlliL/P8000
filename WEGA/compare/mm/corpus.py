"""Corpus of mm (host/recon/corpusrun.py): the original script /usr/bin/mm against src/cmd/mm.sh, at its place in the tree
(INSTALLED), on the cases of compare/mm/tests.txt. mm formats a text with the macros of the memorandum package (-cm or, with -y, -mm) by nroff, for the terminal of the option -T (default 450); it can call tbl (-t), neqn (-e), col and greek."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mm'
ORIG = 'usr/bin/mm'
PROG = 'mm'
CANDIDATES = {'wega': 'src/cmd/mm.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/mm', prefix=PRE)
