"""Corpus of mkdir (host/recon/corpusrun.py): the original /bin/mkdir against the build (src/cmd/mkdir, mkdir.c of System III
with the WEGA sccsid, ccv0 with OLD_D5SA, libc-v1.a), at its place in the tree (INSTALLED: set-user-id root as the original),
on the cases of compare/mkdir/tests.txt (host/recon/casefile.py): directories with and without a path, several, the
messages of the source (arg count, cannot access, cannot make directory), as root and as a user (the owner model of
z8k-emu: mkdir runs as root and gives the new directory to the real user)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mkdir'
ORIG = 'bin/mkdir'
PROG = 'mkdir'
CANDIDATES = {'wega': 'src/cmd/mkdir'}
INSTALLED = True
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/bin/mkdir d')
