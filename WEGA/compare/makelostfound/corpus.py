"""Corpus of makelost+found (host/recon/corpusrun.py): the original csh script /etc/makelost+found against
src/cmd/makelost+found.sh, at its place in the tree (INSTALLED), on the cases of compare/makelostfound/tests.txt. The runner
starts a script whose first byte is # with /bin/csh, as execvp() does."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'makelostfound'
ORIG = 'etc/makelost+found'
PROG = 'makelost+found'
CANDIDATES = {'wega': 'src/cmd/makelost+found.sh'}
INSTALLED = True
TIMEOUT = 600


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/makelost+found', prefix=['user: 0 0', 'time: 600000000'])
