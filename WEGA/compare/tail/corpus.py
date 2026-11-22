"""Corpus of tail (host/recon/corpusrun.py): the original /bin/tail against the build (src/cmd/tail, tail.c of System III
with the WEGA sccsid and the order of its globals, ccv0, libc-v1.a), on the cases of compare/tail/tests.txt
(host/recon/casefile.py): -n and +n with l, b and c, a file and the standard input (a pipe: piped), the messages. Not
in the corpus: -f (loops for ever)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'tail'
ORIG = 'bin/tail'
PROG = 'tail'
CANDIDATES = {'wega': 'src/cmd/tail'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'tail t.in')
