"""Corpus of spell (host/recon/corpusrun.py): the original script /usr/bin/spell against src/cmd/spell.sh, at its place in the tree
(INSTALLED), on the cases of compare/spell/tests.txt. spell lists the words of its input that are not in the dictionary (deroff -w, sort, /usr/lib/spell with the tables of /usr/dict). The history file is a file of the case (H, else the script writes into /usr/dict/spellhist of the tree)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'spell'
ORIG = 'usr/bin/spell'
PROG = 'spell'
CANDIDATES = {'wega': 'src/cmd/spell.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['env: H=hist', 'user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/tmp']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/spell', prefix=PRE)
