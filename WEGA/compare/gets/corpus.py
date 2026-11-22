"""Corpus of gets (host/recon/corpusrun.py): the original /bin/gets against the build (src/cmd/gets, reconstructed from
the binary: setbuf, gets, printf; ccv0, libc-v1.a with the gets of 07/82), on the cases of compare/gets/tests.txt
(host/recon/casefile.py): one line of the standard input, the end of the input with and without an argument."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'gets'
ORIG = 'bin/gets'
PROG = 'gets'
CANDIDATES = {'wega': 'src/cmd/gets'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'gets')
