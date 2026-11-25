"""Corpus of ed (host/recon/corpusrun.py): the original /bin/ed against the build. The cases are the blocks of
compare/ed/tests.txt (host/recon/casefile.py: the format; the default file t.in holds the commands, the default step is
`ed f` with t.in on the standard input): the commands of the man page and of the switch in commands() (ed.c), the
addresses, the regular expressions, the substitutions, the global commands, undo, the files (e, r, w, W, f), the shell
escape and the diagnostics. Every case has an expectation on the original, written from the man page or the source.
Not in the corpus: the encryption (x, X, -x: it runs /usr/lib/makekey), the signals (SIGHUP writes ed.hup, SIGINT), the
limits of the temp file. An error moves the standard input to its end (error(): lseek(0, 0L, 2)): a case has at most one
error, as its last command."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'ed'
ORIG = 'bin/ed'
PROG = 'ed'
CANDIDATES = {'wega': 'src/cmd/ed/ed'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'ed f\nstdin: t.in')
