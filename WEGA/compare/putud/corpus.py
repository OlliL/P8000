"""Corpus of putud (host/recon/corpusrun.py): the original /bin/putud against the shipped build, on the cases of
compare/putud/tests.txt (host/recon/casefile.py): the console and a UDOS disk in drive 0, the 8-bit part behind /dev/ud as
the model host/z8k-emu/models/ud8.py (increment 117). Not in the corpus: names longer than 18 characters (putud builds STX name ETX in bufw[20]: a longer one writes behind it)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'putud'
ORIG = 'bin/putud'
PROG = 'putud'
CANDIDATES = {'wega': 'src/cmd/putud'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'putud t.in')
