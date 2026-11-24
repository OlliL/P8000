"""Corpus of echo2 (host/recon/corpusrun.py): the original /bin/echo2 against the build (src/cmd/echo2.c: the echo of V7 on
stderr, ccv0, libc-v1.a), on the cases of compare/echo2/tests.txt (host/recon/casefile.py): words, -n, no arguments."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'echo2'
ORIG = 'bin/echo2'
PROG = 'echo2'
CANDIDATES = {'wega': 'src/cmd/echo2'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'echo2')
