"""Corpus of calendar (host/recon/corpusrun.py): the original script /usr/bin/calendar against src/cmd/calendar.sh, at its place in the tree
(INSTALLED), on the cases of compare/calendar/tests.txt. calendar looks in the file calendar of the working directory (or, with -, of every user of /etc/passwd, and mails the lines) for the lines with the date of today and tomorrow (the patterns of /usr/lib/calprog). The clock is fixed (Thu Jan 5 1989)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'calendar'
ORIG = 'usr/bin/calendar'
PROG = 'calendar'
CANDIDATES = {'wega': 'src/cmd/calendar.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/tmp']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/calendar', prefix=PRE)
