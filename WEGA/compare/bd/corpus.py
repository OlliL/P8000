"""Corpus of bd (host/recon/corpusrun.py): the original /bin/bd against the build. The cases are the blocks of
compare/bd/tests.txt (host/recon/casefile.py: the format; the default file is a): the man page (a file, the standard input)
and what the source has besides (full lines, the addresses, bytes that are not printable, the last line, the message).
Every case has an expectation on the original, written from the man page or the source, not taken from a run."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'bd'
ORIG = 'bin/bd'
PROG = 'bd'
CANDIDATES = {'wega': 'src/cmd/bd'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 30


def generate(g):
    return casefile.generate(g, TESTS, 'bd a', first='a')
