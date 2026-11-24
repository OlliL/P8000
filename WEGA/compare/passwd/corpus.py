"""Corpus of passwd (host/recon/corpusrun.py): the original /bin/passwd against the build (src/cmd/passwd.c: System III
passwd.c with the lock of WEGA, ccv1, libc-v4.a), on the cases of compare/passwd/tests.txt (host/recon/casefile.py): the
cases that do not change /etc: no login name, an unknown user."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'passwd'
ORIG = 'bin/passwd'
PROG = 'passwd'
CANDIDATES = {'wega': 'src/cmd/passwd'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'passwd')
