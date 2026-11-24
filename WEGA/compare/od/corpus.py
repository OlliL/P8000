"""Corpus of od (host/recon/corpusrun.py): the original /bin/od against the build (src/cmd/od.c: System III od.c with the main
of WEGA, ccv1r, libc-v3.a), also called as hd (ALIASES), on the cases of compare/od/tests.txt (host/recon/casefile.py): the
formats -o -d -x -h -c -b and their mixes, hd's default, the offset +N, the repeated lines (*), a missing file."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'od'
ORIG = 'bin/od'
PROG = 'od'
ALIASES = {'hd': 'bin/hd'}
RC_UNDEFINED = True	# main ends without exit (System III od.c)
CANDIDATES = {'wega': 'src/cmd/od'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'od')
