"""Corpus of uusend (host/recon/corpusrun.py): the original /bin/uusend against the build. The cases are the blocks of
compare/uusend/tests.txt (host/recon/casefile.py: the format; the default step is uusend): the local copies (a file, stdin,
-m, -f into a directory), the usage and the errors. A remote destination (sys!file) runs uux, ~user writes below the
home or the public directory of the tree: not in the corpus. Every case has an expectation on the original, written from
the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'uusend'
ORIG = 'bin/uusend'
PROG = 'uusend'
CANDIDATES = {'wega': 'src/cmd/uucp/uusend'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'uusend')
