"""Corpus of dosls (host/recon/corpusrun.py): the original /usr/bin/dosls against the shipped build (doscopy version 1.1,
src/cmd/doscopy/1.1), on the cases of compare/dosls/tests.txt (host/recon/casefile.py). Man page: /usr/man/man1/dos.1.
The floppy of every case: host/recon/dospre.py; the files on it are put there by the original doscp, dosmkdir."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402
import dospre  # noqa: E402

NAME = 'dosls'
ORIG = 'usr/bin/dosls'
PROG = 'dosls'
CANDIDATES = {'wega': 'src/cmd/doscopy/1.1/dosls'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'dosls /dev/rfd1:', prefix=dospre.PRE)
