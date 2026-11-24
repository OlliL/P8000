"""Corpus of getfile (host/recon/corpusrun.py): the original /bin/getfile against the build (src/cmd/getfile.c, written from
/bin/getfile, the shipped cc and libc), on the cases of compare/getfile/tests.txt (host/recon/casefile.py): the summary
without files, the options, a transfer whose partner answers from the standard input (none, a refusal, a file)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'getfile'
ORIG = 'bin/getfile'
PROG = 'getfile'
CANDIDATES = {'wega': 'src/cmd/getfile'}
TIMEOUT = 30


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'getfile')
