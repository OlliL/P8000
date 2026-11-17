"""Corpus of chkdiff (host/recon/corpusrun.py): the original csh script /bin/chkdiff against src/cmd/chkdiff.sh, at its place in the
tree (INSTALLED), on the cases of compare/chkdiff/tests.txt. The versions are made by the original /bin/chkin."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'chkdiff'
ORIG = 'bin/chkdiff'
PROG = 'chkdiff'
CANDIDATES = {'wega': 'src/cmd/chkdiff.sh'}
INSTALLED = True
TIMEOUT = 120


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/bin/chkdiff', prefix=['user: 0 0', 'time: 600000000'])
