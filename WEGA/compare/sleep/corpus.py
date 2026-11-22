"""Corpus of sleep (host/recon/corpusrun.py): the original /bin/sleep against the build (src/cmd/sleep, sleep.c of System
III with the WEGA sccsid, ccv0, libc-v1.a with the V7 sleep of oldlibc), on the cases of compare/sleep/tests.txt
(host/recon/casefile.py): the argument and the messages of the source, short times only."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'sleep'
ORIG = 'bin/sleep'
PROG = 'sleep'
CANDIDATES = {'wega': 'src/cmd/sleep'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'sleep')
