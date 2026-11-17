"""Corpus of lint (host/recon/corpusrun.py): the original script /usr/bin/lint against src/cmd/lint.sh, at its place in the tree
(INSTALLED), on the cases of compare/lint/tests.txt. lint checks C programs (cpp, lint1, the libraries of /usr/lib, lint2). lint1 leaves files in /usr/tmp: that and /tmp are directories of the layer."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'lint'
ORIG = 'usr/bin/lint'
PROG = 'lint'
CANDIDATES = {'wega': 'src/cmd/lint.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/tmp $Z8K_OVERLAY/usr/tmp']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/lint', prefix=PRE)
