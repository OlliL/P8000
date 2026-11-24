"""Corpus of time (host/recon/corpusrun.py): the original /bin/time against the build (src/cmd/time.c, written from /bin/time,
ccv1, libc-v4.a), on the cases of compare/time/tests.txt (host/recon/casefile.py): no command, -v, a command that fails, a
missing command, the exit status passed on."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'time'
ORIG = 'bin/time'
PROG = 'time'
CANDIDATES = {'wega': 'src/cmd/time'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'time')
