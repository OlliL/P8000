"""Corpus of verify (host/recon/corpusrun.py): the original /bin/verify against the shipped build (version 1.5, the one the
build makes: src/cmd/verify/Makefile), on the cases of compare/verify/tests.txt (host/recon/casefile.py): the console and the
8-bit part behind /dev/ud (host/z8k-emu/models/ud8.py: the requests F, V, C of K.BLK.S on the image files of the
drives)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'verify'
ORIG = 'bin/verify'
PROG = 'verify'
CANDIDATES = {'wega': 'src/cmd/verify/1.5/verify'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'verify')
