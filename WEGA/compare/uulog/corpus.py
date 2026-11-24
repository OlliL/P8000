"""Corpus of uulog (host/recon/corpusrun.py): the original /bin/uulog against the build (src/cmd/uucp: uulog.c and its modules of 3BSD,
uucpname.c, uucp.h of 3BSD with UNAME, the shipped cc), on the cases of compare/uulog/tests.txt (host/recon/casefile.py):
the options that do not touch the spool (the spool of the root is not copied)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'uulog'
ORIG = 'bin/uulog'
PROG = 'uulog'
CANDIDATES = {'wega': 'src/cmd/uucp/uulog'}
PIDS = True
ROOT_COPY = ['usr/spool/uucp/remotelines']	# its directory in the layer: the lock and log files of uulog go there, not into the tree
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'uulog')
