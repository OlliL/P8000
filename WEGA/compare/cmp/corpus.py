"""Corpus of cmp (host/recon/corpusrun.py): the original /bin/cmp against the build (src/cmd/cmp, cmp.c of System III with
the WEGA sccsid, the shipped cc and libc.a), on the cases of compare/cmp/tests.txt (host/recon/casefile.py): -s, -l,
the skips (never read: a quirk), "-" for the standard input, every message of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'cmp'
ORIG = 'bin/cmp'
PROG = 'cmp'
CANDIDATES = {'wega': 'src/cmd/cmp'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'cmp')
