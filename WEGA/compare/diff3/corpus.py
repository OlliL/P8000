"""Corpus of diff3 (host/recon/corpusrun.py): the original script /usr/bin/diff3 against src/cmd/diff3.sh, at its place in the tree
(INSTALLED), on the cases of compare/diff3/tests.txt. diff3 compares three files (diff of file1 and file3, of file2 and file3, the program /usr/lib/diff3prog puts them together)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'diff3'
ORIG = 'usr/bin/diff3'
PROG = 'diff3'
CANDIDATES = {'wega': 'src/cmd/diff3.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/diff3', prefix=PRE)
