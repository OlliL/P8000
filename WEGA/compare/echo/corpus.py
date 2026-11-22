"""Corpus of echo (host/recon/corpusrun.py): the original /bin/echo against the build (src/cmd/echo, echo.c of System III
with -n and the WEGA sccsid, ccv0, libc-v11.a), on the cases of compare/echo/tests.txt (host/recon/casefile.py): every
escape of the source, -n, the octal escape and its limits."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'echo'
ORIG = 'bin/echo'
PROG = 'echo'
CANDIDATES = {'wega': 'src/cmd/echo'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'echo')
