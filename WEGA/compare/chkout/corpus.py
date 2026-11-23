"""Corpus of chkout (host/recon/corpusrun.py): the original /bin/chkout against the build. The cases are the blocks of
compare/chkout/tests.txt (host/recon/casefile.py: the format; the default file is a+, the chkfile of a, the default step
chkout -p a): the man pages chkout(1) and qcv(5) (a version, -v, -h, -p, -e, the files made, the keywords) and what the
source has besides (the diff commands a, c, d of diff -f, the messages and the exit status). Every case has an expectation
on the original, written from the man page or the source, not taken from a run.
Not in the corpus: $D$ (the time of the run in the guest's zone), a chkfile whose first line is not an & line (the original
writes through the null pointer)."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'chkout'
ORIG = 'bin/chkout'
PROG = 'chkout'
CANDIDATES = {'wega': 'src/cmd/sccs/chkout'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 10


def generate(g):
    return casefile.generate(g, TESTS, 'chkout -p a', first='a+')
