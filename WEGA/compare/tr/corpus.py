"""Corpus of tr (host/recon/corpusrun.py): the original /bin/tr against the build (src/cmd/tr.c: System III tr.c with the
usage message of WEGA, the WEGA sccsid, ccv1, libc-v6.a), on the cases of compare/tr/tests.txt (host/recon/casefile.py):
the options -c -d -s, ranges, repeats [a*n], octal escapes, the usage message and the errors."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'tr'
ORIG = 'bin/tr'
PROG = 'tr'
CANDIDATES = {'wega': 'src/cmd/tr'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'tr')
