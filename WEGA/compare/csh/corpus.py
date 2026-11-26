"""Corpus of csh (host/recon/corpusrun.py): the original /bin/csh against the build. The cases are the blocks of
compare/csh/tests.txt (host/recon/casefile.py: the format; the default step is csh t.in: a script). They cover WEGA's
changes (echo escapes and -n, the environment as shell variables, if( and while(, unsetenv, ignoreexit, nice, the
type of a script by its first line, exportpath) and the Berkeley builtins, expressions, globbing, history, aliases and
the errors. Interactive use (prompt, mail, the terminal) and time (its numbers vary) are not in the corpus. Every case
has an expectation on the original, written from the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'csh'
ORIG = 'bin/csh'
PROG = 'csh'
CANDIDATES = {'wega': 'src/cmd/csh/csh'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'csh t.in')
