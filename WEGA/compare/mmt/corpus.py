"""Corpus of mmt (host/recon/corpusrun.py): the original script /usr/bin/mmt against src/cmd/mmt.sh, at its place in the tree
(INSTALLED), on the cases of compare/mmt/tests.txt. mmt formats a text with the memorandum macros by troff; mvt (a link to mmt: the name decides) uses the viewgraph macros (-mv). With -a troff writes the text for the terminal (no typesetter in the emulator: without it troff says "Typesetter busy")."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mmt'
ORIG = 'usr/bin/mmt'
PROG = 'mmt'
CANDIDATES = {'wega': 'src/cmd/mmt.sh'}
INSTALLED = True
TIMEOUT = 120
ALIASES = {'mvt': 'usr/bin/mvt'}
PRE = ['user: 0 0', 'time: 600000000']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/mmt', prefix=PRE)
