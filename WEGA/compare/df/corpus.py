"""Corpus of df (host/recon/corpusrun.py): the original /bin/df against the build (src/cmd/df.c: System III df.c as WEGA changed
it, ccv1, libc-v9.a), on the cases of compare/df/tests.txt (host/recon/casefile.py): the options, an unknown option, the
start (nlist of /wega, /dev/kmem) as the emulator gives it."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'df'
ORIG = 'bin/df'
PROG = 'df'
CANDIDATES = {'wega': 'src/cmd/df'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'df')
