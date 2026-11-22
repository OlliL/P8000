"""Corpus of uname (host/recon/corpusrun.py): the original /bin/uname against the build (src/cmd/uname, uname.c of System
III with the WEGA sccsid, ccv1, libc-v12.a), on the cases of compare/uname/tests.txt (host/recon/casefile.py): every
option of the source and the usage (the values come from the uname of z8k-emu)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'uname'
ORIG = 'bin/uname'
PROG = 'uname'
CANDIDATES = {'wega': 'src/cmd/uname'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'uname')
