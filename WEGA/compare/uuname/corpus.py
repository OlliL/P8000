"""Corpus of uuname (host/recon/corpusrun.py): the original /bin/uuname against the build (src/cmd/uucp: uuname.c of 4BSD,
uucpname.c, uucp.h of 3BSD with UNAME, the shipped cc), on the cases of compare/uuname/tests.txt (host/recon/casefile.py):
the list of /usr/lib/uucp/L.sys of the root, -l (the node name of uname), the usage message."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'uuname'
ORIG = 'bin/uuname'
PROG = 'uuname'
CANDIDATES = {'wega': 'src/cmd/uucp/uuname'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'uuname')
