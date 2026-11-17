"""Corpus of cxref (host/recon/corpusrun.py): the original script /usr/bin/cxref against src/cmd/cxref.sh, at its place in the tree
(INSTALLED), on the cases of compare/cxref/tests.txt. cxref lists the names of the functions defined in C files with their line numbers (grep -n, ex, sort). The script begins with a comment: the shell of the system is csh."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'cxref'
ORIG = 'usr/bin/cxref'
PROG = 'cxref'
CANDIDATES = {'wega': 'src/cmd/cxref.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/cxref', prefix=PRE)
