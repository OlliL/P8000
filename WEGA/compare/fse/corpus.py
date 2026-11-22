"""Corpus of fse (host/recon/corpusrun.py): the original /bin/fse against the shipped build (src/cmd/fse: fsen and the
parameter block of fsepar), on the cases of compare/fse/tests.txt (host/recon/casefile.py): the public interface of the
man page (the options, the commands of the text and the dump mode, the backup, .fse, the metakeys, errmsg of -e) and what
the source has besides (-a, J in the question, the end of the input as F10, the keys of the P8000). fse is a full screen
editor: every case runs with the minimal terminal of the emulator (terminal: console, /dev/tty is it), TERM=P8 (the
terminal of the parameter block; other ones through /etc/termcap), the keys typed by the operator. Each case has its own
directory: .fse and errmsg (fse reads and writes them in its current directory) are files of the case.
Not in the corpus: the end of the input at "Press <ET> to continue" and at "Save before quit ?" (fse waits there for
ever: KNOWLEDGE.md, quirks), fse without TERM (getenv gives 0: strcmp reads the data at address 0), ^UM to a directory without
permission."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'fse'
ORIG = 'bin/fse'
PROG = 'fse'
CANDIDATES = {'wega': 'src/cmd/fse/fse'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'fse -c t.in',
                             prefix=('terminal: console', 'env: TERM=P8'))
