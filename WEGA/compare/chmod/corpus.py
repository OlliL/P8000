"""Corpus of chmod (host/recon/corpusrun.py): the original /bin/chmod against the build (src/cmd/chmod, chmod.c of System
III with the WEGA sccsid, ccv0, libc-v1.a), on the cases of compare/chmod/tests.txt (host/recon/casefile.py): octal and
symbolic modes (who, op, permissions, u g o as copies, several clauses), every message of the source. Every mode keeps
the read bit of the owner (the listing of the files afterwards reads them). Not in the corpus: "can't change" (a file of
another owner)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'chmod'
ORIG = 'bin/chmod'
PROG = 'chmod'
CANDIDATES = {'wega': 'src/cmd/chmod'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'chmod')
