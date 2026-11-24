"""Corpus of whereis (host/recon/corpusrun.py): the original /bin/whereis against the build (src/cmd/whereis.c: 2.9BSD as WEGA changed it,
ccv0, libc-v1.a), on the cases of compare/whereis/tests.txt (host/recon/casefile.py): the options, names found in /bin and /usr/man, a missing name."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'whereis'
ORIG = 'bin/whereis'
PROG = 'whereis'
CANDIDATES = {'wega': 'src/cmd/whereis'}
RC_UNDEFINED = True	# main ends without exit (2.9BSD whereis.c)
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'whereis')
