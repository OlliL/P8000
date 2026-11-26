"""Corpus of sh (host/recon/corpusrun.py): the original /bin/sh against the build. The cases are the blocks of
compare/sh/tests.txt (host/recon/casefile.py: the format; the default step is sh t.in: a script). They cover WEGA's
changes (unset, the type of a script by its first line, the login flag) and the Bourne shell: variables, parameters,
quoting, control structures, functions of test, here documents, redirections, traps, set options and the errors.
Interactive use (prompt, mail) is not in the corpus."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'sh'
ORIG = 'bin/sh'
PROG = 'sh'
CANDIDATES = {'wega': 'src/cmd/sh/sh'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'sh t.in')
