"""Corpus of dd (host/recon/corpusrun.py): the original /bin/dd against the build (src/cmd/dd.c: System III dd.c as WEGA changed
it, ccv1r, libc-v10.a), on the cases of compare/dd/tests.txt (host/recon/casefile.py): the block sizes, skip, seek, count,
the conversions, the errors (a missing file, a bad argument, zero counts)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'dd'
ORIG = 'bin/dd'
PROG = 'dd'
CANDIDATES = {'wega': 'src/cmd/dd'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'dd')
