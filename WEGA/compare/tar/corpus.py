"""Corpus of tar (host/recon/corpusrun.py): the original /bin/tar against the shipped build, on the cases of
compare/tar/tests.txt (host/recon/casefile.py): every function and modifier of the man page and the messages of the
source. tar runs /bin/pwd (the working directory), /bin/mkdir, and for u /bin/sh, sort, awk, mv of the tree. The cases
run in the guest /tmp (IN_TMP), so that pwd finds /tmp/CASE (the emulator's model of the guest /tmp as a mount); every
file has the time 1988-03-04 05:06:07 UTC before the case starts."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'tar'
ORIG = 'bin/tar'
PROG = 'tar'
CANDIDATES = {'wega': 'src/cmd/tar'}
TIMEOUT = 60
IN_TMP = True
TOUCH = "host: find . -exec touch -h -d '1988-03-04 05:06:07 UTC' {} +"


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'tar tf t.in', prefix=[TOUCH])
