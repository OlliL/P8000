"""Corpus of stty (host/recon/corpusrun.py): the original /bin/stty against the build (src/cmd/stty, stty.c of System III with the
WEGA tables, the shipped cc, libc-v15), on the cases of compare/stty/tests.txt (host/recon/casefile.py), on the console of the
emulator (terminal: console): the settings printed (no argument, -a, -g), every mode and speed of the tables accepted, the
control characters, the messages. What a mode sets is not seen: the console of z8k-emu keeps its settings per process (a
host fork), so a second stty does not see what the first one set."""
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


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'stty')
