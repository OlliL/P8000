"""Corpus of doscat (host/recon/corpusrun.py): the original /usr/bin/doscat against the shipped build (doscopy version 1.1,
src/cmd/doscopy/1.1), on the cases of compare/doscat/tests.txt (host/recon/casefile.py). Man page: /usr/man/man1/dos.1.
The floppy of every case: host/recon/dospre.py; the files on it are put there by the original doscp, dosmkdir."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402
import dospre  # noqa: E402

NAME = 'doscat'
ORIG = 'usr/bin/doscat'
PROG = 'doscat'
CANDIDATES = {'wega': 'src/cmd/doscopy/1.1/doscat'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'doscat', prefix=dospre.PRE)
