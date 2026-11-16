"""Corpus of restor (host/recon/corpusrun.py): the original /etc/restor against the shipped build (src/cmd/restor.c), on the
cases of compare/restor/tests.txt (host/recon/casefile.py). The archives are made by the original /etc/dump from the file system
of host/recon/dumppre.py, in the prefix of every case (a case that needs another one makes it with a step of its own)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402
import dumppre  # noqa: E402

NAME = 'restor'
ORIG = 'etc/restor'
PROG = 'restor'
CANDIDATES = {'wega': 'src/cmd/restor'}
TIMEOUT = 120


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/restor', prefix=dumppre.pre() + ['step: /etc/dump 0'])
