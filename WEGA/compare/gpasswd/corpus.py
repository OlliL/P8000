"""Corpus of gpasswd (host/recon/corpusrun.py): the original /bin/gpasswd against the build (src/cmd/gpasswd.c, written
from /bin/gpasswd, the shipped cc, libc-v12.a), on the cases of compare/gpasswd/tests.txt (host/recon/casefile.py): the cases
that do not change /etc: no argument, an unknown group, a group of another user."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'gpasswd'
ORIG = 'bin/gpasswd'
PROG = 'gpasswd'
CANDIDATES = {'wega': 'src/cmd/gpasswd'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'gpasswd')
