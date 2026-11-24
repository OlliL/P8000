"""Corpus of test (host/recon/corpusrun.py): the original /bin/test against the build (src/cmd/test.c: V7 test.c with the
operators of System III sh/test.c, the WEGA sccsid, ccv1, libc-v6.a), on the cases of compare/test/tests.txt
(host/recon/casefile.py): every operator of the source, the errors, -a/-o/!/( ), the exit status."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'test'
ORIG = 'bin/test'
PROG = 'test'
CANDIDATES = {'wega': 'src/cmd/test'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'test')
