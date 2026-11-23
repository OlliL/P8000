"""Corpus of chkin (host/recon/corpusrun.py): the original /bin/chkin against the build. The cases are the blocks of
compare/chkin/tests.txt (host/recon/casefile.py: the format; the default file is a, the default step chkin -c note a):
the man pages chkin(1) and qcv(5) (a new chkfile, a new version with the lock file, -r, -b, -c, -d, the read-only file
with its keywords) and what the source has besides (the messages and the exit status, -v). Every case has an expectation
on the original, written from the man page or the source, not taken from a run. The clock of the guest is fixed
(time:): the &H line holds the time of the run. Not in the corpus: the comments read from a terminal (only when the
standard output is one)."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'chkin'
ORIG = 'bin/chkin'
PROG = 'chkin'
CANDIDATES = {'wega': 'src/cmd/sccs/chkin'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 10


def generate(g):
    return casefile.generate(g, TESTS, 'chkin -c note a', first='a')
