"""Corpus of putfile (host/recon/corpusrun.py): the original /bin/putfile against the build (src/cmd/remote/putfile.c: putfile.c of ZEUS set back to
1.5, ccvm and libc), on the cases of compare/putfile/tests.txt (host/recon/casefile.py): the summary,
the options, a missing and an empty file, a transfer whose partner answers from the standard input."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'putfile'
ORIG = 'bin/putfile'
PROG = 'putfile'
CANDIDATES = {'wega': 'src/cmd/remote/putfile'}
TIMEOUT = 30


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'putfile')
