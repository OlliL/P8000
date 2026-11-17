"""Corpus of install (host/recon/corpusrun.py): the original script /etc/install against src/cmd/install.sh, at its place in the tree
(INSTALLED), on the cases of compare/install/tests.txt. Run as root (chown). The directories it installs into are those of the case
(relative names) or directories of the layer over the tree (usr/bin, ...: the default places it searches with find); the tree itself
is never written."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'install'
ORIG = 'etc/install'
PROG = 'install'
CANDIDATES = {'wega': 'src/cmd/install.sh'}
INSTALLED = True
TIMEOUT = 300


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/install', prefix=['user: 0 0', 'time: 600000000'])
