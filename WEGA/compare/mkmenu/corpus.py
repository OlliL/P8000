"""Corpus of mkmenu (host/recon/corpusrun.py): the original script /usr/bin/mkmenu against src/cmd/mkmenu.sh, at its place in the tree
(INSTALLED), on the cases of compare/mkmenu/tests.txt. mkmenu makes a menu file (a csh script that asks for the header and the entries with gets and writes them as name:type:prompt lines; with the name of the menu as the argument it does not ask for it)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mkmenu'
ORIG = 'usr/bin/mkmenu'
PROG = 'mkmenu'
CANDIDATES = {'wega': 'src/cmd/mkmenu.sh'}
INSTALLED = True
TIMEOUT = 120
IN_TMP = True
PRE = ['user: 0 0', 'time: 600000000', 'env: HOME=/']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/mkmenu', prefix=PRE)
