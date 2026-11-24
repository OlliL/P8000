"""Corpus of date (host/recon/corpusrun.py): the original /bin/date against the build (src/cmd/date.c: System III date.c as WEGA
changed it, ccv1, libc-v6.a), on the cases of compare/date/tests.txt (host/recon/casefile.py): the output (the emulator's
clock), -u, every format character, a bad format character, the usage, setting the time (no permission as a user)."""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'date'
ORIG = 'bin/date'
PROG = 'date'
CANDIDATES = {'wega': 'src/cmd/date'}
TIMEOUT = 60


def after_step(argv, rc, out, err, rd):
    """z8k-emu has no ftime: its message names the address of the call, which moves with the code"""
    return out, re.sub(r'unimplemented system call (\d+) at [0-9a-f]+:[0-9a-f]+', r'unimplemented system call \1', err)


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'date')
