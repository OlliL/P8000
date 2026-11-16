"""Corpus of dosdir (host/recon/corpusrun.py): the original /usr/bin/dosdir against the shipped build (doscopy version 1.0,
src/cmd/doscopy/1.0), on the cases of compare/dosdir/tests.txt (host/recon/casefile.py). Man page: /usr/man/man1/dos.1.
The floppy of every case: host/recon/dospre.py; the files on it are put there by the original doscp, dosmkdir."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402
import dospre  # noqa: E402

NAME = 'dosdir'
ORIG = 'usr/bin/dosdir'
PROG = 'dosdir'
CANDIDATES = {'wega': 'src/cmd/doscopy/1.0/dosdir'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'dosdir', prefix=dospre.PRE)
