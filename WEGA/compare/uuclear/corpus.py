"""Corpus of uuclear (host/recon/corpusrun.py): the original script /bin/uuclear against src/cmd/uuclear.sh, at its place in the tree
(INSTALLED), on the cases of compare/uuclear/tests.txt. The spool directory /usr/spool/uucp is a directory of the layer over the
tree (made by the case, with the files that are there at the start); the original /usr/lib/uucp/uuclean runs in it."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'uuclear'
ORIG = 'bin/uuclear'
PROG = 'uuclear'
CANDIDATES = {'wega': 'src/cmd/uuclear.sh'}
INSTALLED = True
TIMEOUT = 120
SPOOL = '$Z8K_OVERLAY/usr/spool/uucp'


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/bin/uuclear',
                             prefix=['user: 0 0', 'time: 600000000', 'host: mkdir -p ' + SPOOL + ' $Z8K_OVERLAY/tmp'])
