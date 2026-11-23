"""Corpus of touch (host/recon/corpusrun.py): the original /bin/touch against the build (src/cmd/touch, touch.c of System
III with the WEGA sccsid, ccv1, libc-v6.a with the ctime of System III), on the cases of compare/touch/tests.txt
(host/recon/casefile.py): -a, -m, -c, a date and the messages (the times are fixed by the time: of the case)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'touch'
ORIG = 'bin/touch'
PROG = 'touch'
CANDIDATES = {'wega': 'src/cmd/touch'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'touch t.in')
