"""Corpus of mail (host/recon/corpusrun.py): the original /bin/mail against the build (src/cmd/mail.c: System III mail.c
as WEGA changed it, the shipped cc, libc-v12.a), on the cases of compare/mail/tests.txt (host/recon/casefile.py): the usage,
reading a mail file of the case (-f) with the commands of the prompt, an empty file."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mail'
ORIG = 'bin/mail'
PROG = 'mail'
ALIASES = {'rmail': 'bin/rmail'}
CANDIDATES = {'wega': 'src/cmd/mail'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'mail')
