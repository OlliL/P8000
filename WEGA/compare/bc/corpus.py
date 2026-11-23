"""Corpus of bc (host/recon/corpusrun.py): the original /bin/bc against the build. The cases are the blocks of
compare/bc/tests.txt (host/recon/casefile.py: the format; the default file is t.in, the default step bc t.in): the public
interface of the man page (operators, scale, ibase, obase, statements, functions, arrays, -c, -l, files) and what the
source has besides (-d, the messages, the reserved words by two letters, strings, the line continuation, bundle with more
than five elements). bc runs /bin/dc of the WEGA root through a pipe (fork, execl), -c and -d not.
Every case has an expectation on the original, written from the man page or the source, not taken from a run.
Not in the corpus: an interrupt (bc ignores SIGINT), "program too big", "string space exceeded", "bundling space exceeded"
(limits of the tables), an array as an argument (dc of the WEGA root ends with "out of space: redef")."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'bc'
ORIG = 'bin/bc'
PROG = 'bc'
CANDIDATES = {'wega': 'src/cmd/bc/bc'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 30


def generate(g):
    return casefile.generate(g, TESTS, 'bc t.in', first='t.in')
