"""Corpus of tee (host/recon/corpusrun.py): the original /bin/tee against the build (src/cmd/tee, tee.c of System III with
the WEGA sccsid, ccv1, the shipped libc.a), on the cases of compare/tee/tests.txt (host/recon/casefile.py): the options
-a, -i and -, files, the message of the source. Its own puts writes to the standard error."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'tee'
ORIG = 'bin/tee'
PROG = 'tee'
CANDIDATES = {'wega': 'src/cmd/tee'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'tee')
