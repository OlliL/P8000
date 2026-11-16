"""Corpus of dump (host/recon/corpusrun.py): the original /bin/dump... against the shipped build (src/cmd/dump.c), on the cases
of compare/dump/tests.txt (host/recon/casefile.py): the file system dumped is an image of the case (device: rmd4 image:z.img,
/dev/rz, made by host/tools/wegafs.py), the archive a floppy image (drive 1, /dev/dumpdev), /etc/ddate a file of the case in the layer."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'dump'
ORIG = 'etc/dump'
PROG = 'dump'
CANDIDATES = {'wega': 'src/cmd/dump'}
TIMEOUT = 120

import dumppre  # noqa: E402


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'dump', prefix=dumppre.pre())
