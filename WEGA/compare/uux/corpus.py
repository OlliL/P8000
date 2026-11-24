"""Corpus of uux (host/recon/corpusrun.py): the original /bin/uux against the build (src/cmd/uucp: uux.c and its modules of 3BSD with the changes of WEGA,
uucpname.c, uucp.h of 3BSD with UNAME, the shipped cc), on the cases of compare/uux/tests.txt (host/recon/casefile.py):
the usage and an unknown system (the spool writes go to the layer)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'uux'
ORIG = 'bin/uux'
PROG = 'uux'
CANDIDATES = {'wega': 'src/cmd/uucp/uux'}
PIDS = True
ROOT_COPY = ['usr/spool/uucp/remotelines', 'usr/lib/uucp/L.sys']	# its directory in the layer: the lock and log files of uux go there, not into the tree
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'uux')
