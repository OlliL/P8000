"""Corpus of dc (host/recon/corpusrun.py): the original /bin/dc against the shipped build. The cases are the blocks of
compare/dc/tests.txt (host/recon/casefile.py: the format; the default file is t.dc, the default step dc t.dc): the public interface of the man page (every command, the example,
the diagnostics) and what the source has besides (Y, Q, K, !< !> !=, the registers of 0241 and above, the limits of the
stack, the nesting and the symbol table, the syntax of numbers, the line continuation). Every case has an expectation on
the original, written from the man page or the source, not taken from a run.
Not in the corpus: an interrupt (SIGINT: onintr restarts the command loop; the emulator does not run guest signal handlers),
"out of space" (it needs the whole data space and ends in abort()), Y on an empty stack (it dumps whatever lies at the
address 0 of the data: different in every binary)."""
import os
import re
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'dc'
ORIG = 'bin/dc'
PROG = 'dc'
CANDIDATES = {'wega': 'src/cmd/dc/dc'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 30	# two-points loops for ever (a bug of the original): it ends here

# Y (sdump) prints the addresses of a header and its string in octal: they depend on the layout of the binary
_ADDR = re.compile(r'^(stk|elt|odd beg) \d+ rd \d+ wt \d+ beg \d+ last \d+$', re.M)


def normalize(what, data):
    if what == 'output':
        return _ADDR.sub(r'\1 ADDR rd ADDR wt ADDR beg ADDR last ADDR', data)
    return data


def generate(g):
    return casefile.generate(g, TESTS, 'dc t.dc', first='t.dc')
