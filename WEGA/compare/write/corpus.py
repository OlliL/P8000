"""Corpus of write (host/recon/corpusrun.py): the original /bin/write against the shipped build (src/cmd/write.c), on the cases
of compare/write/tests.txt (host/recon/casefile.py). The terminals of the other users are image files (device: ttyN
image:FILE), /etc/utmp is made by the case in the layer over the tree (host/tools/mkutmp.py)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'write'
ORIG = 'bin/write'
PROG = 'write'
CANDIDATES = {'wega': 'src/cmd/write'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'write', prefix=['time: 600000000'])
