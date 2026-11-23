"""Corpus of mkstr (host/recon/corpusrun.py): the original /usr/bin/mkstr against the build (src/cmd/mkstr, mkstr.c of
Berkeley with the WEGA sccsid, the globals in the order of the original and the label in main; ccv0, libc-v11.a), on the
cases of compare/mkstr/tests.txt (host/recon/casefile.py): the strings of error("...") moved into the message file
(with their escapes and the hash of old messages), "-" to append, several files, the usage and the messages."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mkstr'
ORIG = 'usr/bin/mkstr'
PROG = 'mkstr'
CANDIDATES = {'wega': 'src/cmd/mkstr'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'mkstr strs x a.c')
