"""Corpus of dosformat (host/recon/corpusrun.py): the original /usr/bin/dosformat against the shipped build (doscopy version
1.0, src/cmd/doscopy/1.0), on the cases of compare/dosformat/tests.txt (host/recon/casefile.py). Man page:
/usr/man/man1/dos.1. On a floppy device dosformat runs /bin/format (the 8-bit part behind /dev/ud, the console) and writes the
boot sector, the FATs and the directory; on a name that is not a device it writes only those into a file (the image files of
the cases)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402
import dospre  # noqa: E402

NAME = 'dosformat'
ORIG = 'usr/bin/dosformat'
PROG = 'dosformat'
CANDIDATES = {'wega': 'src/cmd/doscopy/1.0/dosformat'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'dosformat', prefix=dospre.pre(False))
