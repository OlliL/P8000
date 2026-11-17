"""Corpus of UGETT1 (host/recon/corpusrun.py): the original script /etc/UGETT1 against src/cmd/UGETT1.sh, at its place in the tree
(INSTALLED), on the cases of compare/UGETT1/tests.txt. The script runs the original /etc/GETTY on a line and then empties the
file /usr/spool/uucp/GET.LINE of the spool directory, which is a directory of the layer."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'UGETT1'
ORIG = 'etc/UGETT1'
PROG = 'UGETT1'
CANDIDATES = {'wega': 'src/cmd/UGETT1.sh'}
INSTALLED = True
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/UGETT1',
                             prefix=['user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/usr/spool/uucp $Z8K_OVERLAY/etc $Z8K_OVERLAY/usr/adm $Z8K_OVERLAY/tmp'])
