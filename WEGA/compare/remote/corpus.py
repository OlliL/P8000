"""Corpus of remote (host/recon/corpusrun.py): the original /bin/remote against the build (src/cmd/remote/remote.c: remote.c
of ZEUS set back to 2.8, ccvm and libc), on the cases of compare/remote/tests.txt (host/recon/casefile.py): the line
database /usr/spool/uucp/remotelines (a file of the layer over the tree, made by the case) without the system asked
for, with a line that cannot be parsed, empty. A connection itself needs a second system on a line: not in the corpus."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'remote'
ORIG = 'bin/remote'
PROG = 'remote'
CANDIDATES = {'wega': 'src/cmd/remote/remote'}
TIMEOUT = 30
SPOOL = '$Z8K_OVERLAY/usr/spool/uucp'


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'remote',
                             prefix=['host: mkdir -p ' + SPOOL])
