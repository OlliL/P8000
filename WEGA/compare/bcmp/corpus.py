"""Corpus of bcmp (host/recon/corpusrun.py): the original /bin/bcmp against the build. The cases are the blocks of
compare/bcmp/tests.txt (host/recon/casefile.py: the format; the default file is a, the default step bcmp a b): the man
page (equal files, differences, a file that is part of the other) and what the source has besides (which file ends first,
bytes that are not printable, the messages). Every case has an expectation on the original, written from the man page or
the source, not taken from a run.
Not in the corpus: bcmp with one argument (file1 is the standard input and file2 the null pointer) or none (fopen of the
null pointer): what they do depends on the bytes at address 0 of the data."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'bcmp'
ORIG = 'bin/bcmp'
PROG = 'bcmp'
CANDIDATES = {'wega': 'src/cmd/bcmp'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 30


def generate(g):
    return casefile.generate(g, TESTS, 'bcmp a b', first='a')
