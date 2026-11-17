"""Corpus of halt (host/recon/corpusrun.py): the original csh script /etc/halt against src/cmd/halt.sh, at its place in the tree
(INSTALLED), on the cases of compare/halt/tests.txt. /etc is a directory of the layer (the case's motd, utmp), the terminals of the
logged in users are image files (device: ttyN image:FILE). The script waits 30 seconds (sleep 30)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'halt'
ORIG = 'etc/halt'
PROG = 'halt'
CANDIDATES = {'wega': 'src/cmd/halt.sh'}
INSTALLED = True
ROOT_COPY = ['etc/motd']
TIMEOUT = 120


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/halt',
                             prefix=['user: 0 0', 'time: 600000000', 'host: : > t2; python3 $WEGA_HOST/tools/mkutmp.py $Z8K_OVERLAY/etc/utmp console:me tty2:bob',
                                     'device: tty2 image:t2'])
