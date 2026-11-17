"""Corpus of strprint (host/recon/corpusrun.py): the original script /usr/bin/strprint against src/cmd/strprint.sh, at its place in the tree
(INSTALLED), on the cases of compare/strprint/tests.txt. strprint prints the problem files *.prob* (a csh script: the names are those of the working directory, the files are looked for in /usr/lib/problems, put through pr into one file and given to lpr). lpr is a script in the layer that prints the file; the clock is fixed (pr heads with the time of the file)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'strprint'
ORIG = 'usr/bin/strprint'
PROG = 'strprint'
CANDIDATES = {'wega': 'src/cmd/strprint.sh'}
INSTALLED = True
PIDS = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/tmp $Z8K_OVERLAY/usr/bin $Z8K_OVERLAY/usr/lib/problems', "host: printf '#!/bin/sh\\ncat \"$@\"\\n' > $Z8K_OVERLAY/usr/bin/lpr; chmod 755 $Z8K_OVERLAY/usr/bin/lpr"]


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/strprint', prefix=PRE)
