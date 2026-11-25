"""Corpus of stty (host/recon/corpusrun.py): the original /bin/stty against the build (src/cmd/stty, stty.c of System III with the
WEGA tables, the shipped cc, libc-v15), on the cases of compare/stty/tests.txt (host/recon/casefile.py), on the console of the
emulator (terminal: console, CONSOLE_KEEP: the console of the case, what a step sets a later one sees): every mode and speed of
the tables set and the settings printed (-g), from sane and from raw; no argument, -a, the control characters, a shell
with several stty, the messages."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'stty'
ORIG = 'bin/stty'
PROG = 'stty'
CANDIDATES = {'wega': 'src/cmd/stty'}
TIMEOUT = 60
CONSOLE_KEEP = True    # what a step sets, a later step sees (z8k-emu Z8K_CONSOLE_STATE)


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'stty')
