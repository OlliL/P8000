"""Corpus of mvdir (host/recon/corpusrun.py): the original script /etc/mvdir against src/cmd/mvdir.sh, at its place in the tree
(INSTALLED: the layer over ROOT of each case), on the cases of compare/mvdir/tests.txt (host/recon/casefile.py). The script
runs the original /etc/link and /etc/unlink (the link of a directory is a rename in the emulator, host/z8k-emu/README.md) as
the super-user, which link needs for a directory. mv (compare/mv) has the program's own mvdir code and the set-user-id case."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mvdir'
ORIG = 'etc/mvdir'
PROG = 'mvdir'
CANDIDATES = {'wega': 'src/cmd/mvdir.sh'}
INSTALLED = True
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/mvdir', prefix=['user: 0 0', 'time: 600000000'])
