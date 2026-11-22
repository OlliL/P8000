"""Corpus of du (host/recon/corpusrun.py): the original /bin/du against the build (src/cmd/du, du.c of System III with the
WEGA sccsid, the shipped cc and libc.a), on the cases of compare/du/tests.txt (host/recon/casefile.py): -a, -r, -s,
files and trees, links (counted once), the messages of the source (the sizes of directories are those the emulator
gives)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'du'
ORIG = 'bin/du'
PROG = 'du'
CANDIDATES = {'wega': 'src/cmd/du'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'du')
