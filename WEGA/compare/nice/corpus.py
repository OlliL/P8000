"""Corpus of nice (host/recon/corpusrun.py): the original /bin/nice against the build (src/cmd/nice.c: V7 nice.c with exit(2),
the WEGA sccsid, ccv1, libc-v4.a), on the cases of compare/nice/tests.txt (host/recon/casefile.py): the usage, -n, the path
search of execvp (a command, a shell script, a missing command)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'nice'
ORIG = 'bin/nice'
PROG = 'nice'
CANDIDATES = {'wega': 'src/cmd/nice'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'nice')
