"""Corpus of rm (host/recon/corpusrun.py): the original /bin/rm against the shipped build, on the cases of
compare/rm/tests.txt (host/recon/casefile.py): every option of the man page and what the source has besides (rm -i
alone, the count of errors as exit status, the status of rmdir added to it). rm -r runs /bin/rmdir of the tree (the
original on both sides). Without a terminal rm sets -f itself; the cases with terminal: console and user: ask
(the user cases). Not in the corpus: a
directory in rm -i alone (its question is printf without its argument), names longer than the buffer of 100."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'rm'
ORIG = 'bin/rm'
PROG = 'rm'
CANDIDATES = {'wega': 'src/cmd/rm'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'rm t.in')
