"""Corpus of clist (host/recon/corpusrun.py): the original /lib/clist against the build, on the cases of
compare/clist/tests.txt (host/recon/casefile.py): every message and branch of the source (no man page; cc -Sl calls
"clist file.s file.o")."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'clist'
ORIG = 'lib/clist'
PROG = 'clist'
CANDIDATES = {'wega': 'src/cmd/clist'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'clist t.in t.out')
