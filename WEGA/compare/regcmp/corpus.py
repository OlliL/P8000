"""Corpus of regcmp (host/recon/corpusrun.py): the original /bin/regcmp against the build (src/cmd/regcmp.c of System III with
the WEGA changes, ccv1, libPW, libc-v4.a), on the cases of compare/regcmp/tests.txt (host/recon/casefile.py): files of
regular expressions (the .i file or stdout with -), the errors of the input and of the expressions."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'regcmp'
ORIG = 'bin/regcmp'
PROG = 'regcmp'
CANDIDATES = {'wega': 'src/cmd/regcmp'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'regcmp')
