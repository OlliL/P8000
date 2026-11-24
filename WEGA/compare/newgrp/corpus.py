"""Corpus of newgrp (host/recon/corpusrun.py): the original /bin/newgrp against the build (src/cmd/newgrp.c: V7 newgrp.c as WEGA
changed it, the shipped cc, libc-v12.a), on the cases of compare/newgrp/tests.txt (host/recon/casefile.py): the usage, an
unknown group, a group of the root's /etc/group, no argument; the shell started gets exit from its input."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'newgrp'
ORIG = 'bin/newgrp'
PROG = 'newgrp'
CANDIDATES = {'wega': 'src/cmd/newgrp'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'newgrp')
