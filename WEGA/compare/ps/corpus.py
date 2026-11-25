"""Corpus of ps (host/recon/corpusrun.py): the original /bin/ps against the build. The cases are the blocks of
compare/ps/tests.txt (host/recon/casefile.py: the format; the default step is ps). The emulator has no /dev/kmem and
/dev/mem (increment 162), so without -k ps stops with "ps: no mem"; with -k it reads a crash dump that the case makes
by host (compare/ps/mkdump.py: the MMU table, the proc table with six processes, their user blocks): the listings -e,
-f, -l, the selections -p -u -g -t -a -d, the zombie, the swapped process. The usage and the error messages too.
Every case has an expectation on the original, written from the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'ps'
ORIG = 'bin/ps'
PROG = 'ps'
CANDIDATES = {'wega': 'src/cmd/ps/ps'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'ps')
