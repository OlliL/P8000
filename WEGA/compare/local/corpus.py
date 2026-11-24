"""Corpus of local (host/recon/corpusrun.py): the original /bin/local against the build (src/cmd/remote/local.c: local.c of
ZEUS rewritten to the WEGA version 1.5 of 02/28/84, the shipped cc and libc), on the cases of compare/local/tests.txt
(host/recon/casefile.py). The local system (the partner) answers on the standard input: the quit request, the logout
request (-l), a command (the X request: the arguments joined, + and % change the case) with ACK, ENQ and an exit status,
NAKs up to the retries."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'local'
ORIG = 'bin/local'
PROG = 'local'
CANDIDATES = {'wega': 'src/cmd/remote/local'}
TIMEOUT = 30


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'local')
