"""Corpus of nl (host/recon/corpusrun.py): the original /bin/nl against the build (src/cmd/nl, nl.c of System III with
the WEGA changes: -nln, the digits 0 and 9 in the numbers of options, the sccsid, an unused word in main; the shipped
cc and libc.a), on the cases of compare/nl/tests.txt (host/recon/casefile.py): the options of the source and the
sections (\:\:\:, \:\:, \:)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'nl'
ORIG = 'bin/nl'
PROG = 'nl'
CANDIDATES = {'wega': 'src/cmd/nl'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'nl t.in')
