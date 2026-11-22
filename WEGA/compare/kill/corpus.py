"""Corpus of kill (host/recon/corpusrun.py): the original /bin/kill against the build (src/cmd/kill, kill.c of System III
with the WEGA sccsid, ccv0, libc-v1.a), on the cases of compare/kill/tests.txt (host/recon/casefile.py): the arguments
and messages of the source. Only numbers that no guest process has (corpusrun sets Z8K_KILLLOG: z8k-emu grants such a
signal and writes it down; a signal number of 20 and above: "no such process") and the usage: no signal reaches a
process, and no pid 0 or below (the emulator would give it to the host)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'kill'
ORIG = 'bin/kill'
PROG = 'kill'
CANDIDATES = {'wega': 'src/cmd/kill'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'kill')
