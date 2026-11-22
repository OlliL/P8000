"""Corpus of cat (host/recon/corpusrun.py): the original /bin/cat against the build (src/cmd/cat, cat.c of System III with
the WEGA sccsid, ccv0, libc-v11.a), on the cases of compare/cat/tests.txt (host/recon/casefile.py): the options of the
source (-u, -s, -), files and the standard input, every message that can be reached. Not in the corpus: "input is
output" and "Cannot stat stdout" (the standard output of a step is a pipe), "output error" and "close error"."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'cat'
ORIG = 'bin/cat'
PROG = 'cat'
CANDIDATES = {'wega': 'src/cmd/cat'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'cat t.in')
