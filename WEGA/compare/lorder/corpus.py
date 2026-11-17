"""Corpus of lorder (host/recon/corpusrun.py): the original script /usr/bin/lorder against src/cmd/lorder.sh, at its place in the tree
(INSTALLED), on the cases of compare/lorder/tests.txt. lorder prints the pairs "file1 file2" of object files (file1 uses a symbol defined in file2) for ar/ld (nm -g, sed, sort, join)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'lorder'
ORIG = 'usr/bin/lorder'
PROG = 'lorder'
CANDIDATES = {'wega': 'src/cmd/lorder.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/tmp']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/lorder', prefix=PRE)
