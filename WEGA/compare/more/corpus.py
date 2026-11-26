"""Corpus of more (host/recon/corpusrun.py): the original /bin/more (link page) against the build. The cases are the
blocks of compare/more/tests.txt (host/recon/casefile.py: the format; the default step is more a). Rudimentary: WEGA's
more first reopens the standard error for reading (its commands come from there) unless it is open for reading and
writing; the corpus has no terminal for it, so every run ends there (exit 5). The paging itself is not reached.
Every case has an expectation on the original, written from the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'more'
ORIG = 'bin/more'
PROG = 'more'
CANDIDATES = {'wega': 'src/cmd/more/more'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'more a', first='a')
