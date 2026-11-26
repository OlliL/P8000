"""Corpus of uucp (host/recon/corpusrun.py): the original /bin/uucp against the build (src/cmd/uucp: uucp.c and logent.c of
3BSD with the changes of WEGA, the modules of uux, the shipped cc), on the cases of compare/uucp/tests.txt
(host/recon/casefile.py): the usage, an unknown flag, an unknown system and a local copy, as root (-e runs uux and uuxqt: not stable, no case)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'uucp'
ORIG = 'bin/uucp'
PROG = 'uucp'
CANDIDATES = {'wega': 'src/cmd/uucp/uucp'}
PIDS = True
ROOT_COPY = ['usr/spool/uucp/remotelines', 'usr/lib/uucp/L.sys', 'usr/lib/uucp/USERFILE']	# the lock and log files of uucp go to the layer
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'uucp')
