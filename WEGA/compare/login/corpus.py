"""Corpus of login (host/recon/corpusrun.py): the original /bin/login against the build. The cases are the blocks of
compare/login/tests.txt (host/recon/casefile.py: the format; the default step is login), all on the console
(terminal: console: ttyname, getpass). Only what ends before a user is logged in: unknown names, wrong passwords, the
name from argv, the - before the name, a NUL, blanks, long names, the end of the input. A successful login needs the
emulator features of increment 175 (utmp, chown of the tty, setgid/setuid, chroot, the exec of the shell).
Every case has an expectation on the original, written from the source."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'login'
ORIG = 'bin/login'
PROG = 'login'
CANDIDATES = {'wega': 'src/cmd/login'}
TESTS = os.path.join(HERE, 'tests.txt')


def generate(g):
    return casefile.generate(g, TESTS, 'login')
