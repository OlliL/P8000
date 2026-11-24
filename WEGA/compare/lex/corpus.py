"""Corpus of lex (host/recon/corpusrun.py): the original /usr/bin/lex against the build (src/cmd/lex: System III lex,
ccv1l, libc-v3), on the cases of compare/lex/tests.txt (host/recon/casefile.py): specifications with definitions,
character classes, start conditions, trailing context, REJECT-free actions, the options -t -v -n -f -c -r, errors (unknown
option, no input file, bad specification); the output lex.yy.c (or stdout with -t) and the statistics of -v."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'lex'
ORIG = 'usr/bin/lex'
PROG = 'lex'
CANDIDATES = {'wega': 'src/cmd/lex/lex'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'lex t.in')
