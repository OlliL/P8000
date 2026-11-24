"""Corpus of grep (host/recon/corpusrun.py): the original /bin/grep against the build (src/cmd/grep, grep.c of System III
with the WEGA sccsid in front of regexp.h, ccvm and libc.a), on the cases of compare/grep/tests.txt
(host/recon/casefile.py): every option of the source (-b -c -l -n -s -v -y), several files, the standard input, the
regular expressions of regexp.h and the messages."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'grep'
ORIG = 'bin/grep'
PROG = 'grep'
CANDIDATES = {'wega': 'src/cmd/grep'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'grep apple t.in')
