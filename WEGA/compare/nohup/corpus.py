"""Corpus of nohup (host/recon/corpusrun.py): the original /bin/nohup against the build (src/cmd/nohup, nohup.c of System
III with the WEGA sccsid, ccv0, libc-v1.a), on the cases of compare/nohup/tests.txt (host/recon/casefile.py): a command
with its arguments, the usage and a command that is not there. The standard output of a step is a pipe: no nohup.out
(the terminal cases are not in the corpus)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'nohup'
ORIG = 'bin/nohup'
PROG = 'nohup'
CANDIDATES = {'wega': 'src/cmd/nohup'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'nohup')
