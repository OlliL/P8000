"""Corpus of who (host/recon/corpusrun.py): the original /bin/who against the build (src/cmd/who.c: System III who.c with -g,
ccvm and libc), on the cases of compare/who/tests.txt (host/recon/casefile.py): /etc/utmp of the root, a utmp
file of the case, -g, an illegal option, who am i (no terminal)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'who'
ORIG = 'bin/who'
PROG = 'who'
CANDIDATES = {'wega': 'src/cmd/who'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'who')
