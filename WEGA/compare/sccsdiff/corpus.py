"""Corpus of sccsdiff (host/recon/corpusrun.py): the original script /usr/bin/sccsdiff against src/cmd/sccsdiff.sh, at its place in the tree
(INSTALLED), on the cases of compare/sccsdiff/tests.txt. sccsdiff compares two versions of SCCS files (get -p, bdiff; with -p through pr). The SCCS files are made in the case by the originals admin, get and delta (the user is root: they need a user in the password file); pr heads its pages with the clock, fixed here."""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'sccsdiff'
ORIG = 'usr/bin/sccsdiff'
PROG = 'sccsdiff'
CANDIDATES = {'wega': 'src/cmd/sccsdiff.sh'}
INSTALLED = True
TIMEOUT = 120

PRE = ['user: 0 0', 'time: 600000000', 'host: mkdir -p $Z8K_OVERLAY/tmp']


def normalize(what, data):
    """the head of pr: the time of the temporary file is the time of the host (the files of the guest have the clock of the host)"""
    if isinstance(data, str):
        return re.sub(r'[A-Z][a-z]{2} [ 0-9]{2} [0-9]{2}:[0-9]{2} [0-9]{4}', 'DATE', data)
    return data


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/usr/bin/sccsdiff', prefix=PRE)
