"""Corpus of getud (host/recon/corpusrun.py): the original /bin/getud against the shipped build, on the cases of
compare/getud/tests.txt (host/recon/casefile.py): the console and a UDOS disk in drive 0, the 8-bit part behind /dev/ud as
the model host/z8k-emu/models/ud8.py (increment 117). Not in the corpus: an interrupt (DEL: the guest has no signal
handlers), names longer than 18 characters (getud builds STX name ETX in bufw[20]: a longer one writes behind it), a block with a wrong checksum (getud answers it with CAN, not NAK: the 8-bit part ends the file)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'getud'
ORIG = 'bin/getud'
PROG = 'getud'
CANDIDATES = {'wega': 'src/cmd/getud'}
TIMEOUT = 20	# unable-to-create waits for ever (a bug of getud)


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'getud t.in')
