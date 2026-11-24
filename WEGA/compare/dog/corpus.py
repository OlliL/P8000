"""Corpus of dog (host/recon/corpusrun.py): the original /bin/dog against the build (src/cmd/dog.c, written from /bin/dog,
ccv1, libc-v6.a), on the cases of compare/dog/tests.txt (host/recon/casefile.py): short and long files, several files, stdin,
a missing file (the answers come from the standard error, which is no terminal here: 11 lines each)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'dog'
ORIG = 'bin/dog'
PROG = 'dog'
CANDIDATES = {'wega': 'src/cmd/dog'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'dog')
