"""Corpus of chkwhat (host/recon/corpusrun.py): the original /bin/chkwhat against the build. The cases are the blocks of
compare/chkwhat/tests.txt (host/recon/casefile.py: the format; the default file is a, the default step chkwhat a): the man
page (one file, several, -w) and what the source has besides (the ends of a string, the messages and the exit status,
the pattern that is not tested again). Every case has an expectation on the original, written from the man page or the
source, not taken from a run.
Not in the corpus: chkwhat without any argument (it reads the first byte of argv[1], the null pointer), @[$] at the end of a
file with no end of the string behind it (the loop takes EOF as a character and writes it for ever)."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'chkwhat'
ORIG = 'bin/chkwhat'
PROG = 'chkwhat'
CANDIDATES = {'wega': 'src/cmd/sccs/chkwhat'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 10


def generate(g):
    return casefile.generate(g, TESTS, 'chkwhat a', first='a')
