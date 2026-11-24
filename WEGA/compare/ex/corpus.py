"""Corpus of ex (host/recon/corpusrun.py): the original /bin/ex against the build (src/cmd/ex: ex/vi 2.13 of Plexus with the
edits of WEGA, ccv0a, the old cas, shrink.sh, libc-v11.a, libtermlib-v1.a), on the cases of compare/ex/tests.txt
(host/recon/casefile.py): ex in line mode with the commands on stdin (not a terminal: no prompt), the file of the case as
t.in; vi and view with the minimal terminal (terminal: console, TERM=P8 of /etc/termcap), the keys typed by the operator.
EXINIT sets directory=. so that the temporary file lies in the case directory, not in the /tmp of the tree. A non-terminal
stdin ends the commands at the first error (exit 0): one case per error. Not in the corpus: ex in line mode on the console
with typed keys (the step gives no output and runs into the timeout; by hand with Z8K_CONSOLE it works: not looked at),
the recovery (expreserve, exrecover are not built), the confirmation of s///c (reads the terminal).
The candidate of checkall is the build by the shipped cc (make shipped: cc, /lib/libc.a, /usr/lib/libtermlib.a, the shipped
cas); with -DSHIPPED dosub computes sp - genbuf + linebuf, which that cc does not fold to 0."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'ex'
ORIG = 'bin/ex'
PROG = 'ex'
ALIASES = {'edit': 'bin/edit', 'vi': 'bin/vi', 'view': 'bin/view'}
CANDIDATES = {'wega': 'src/cmd/ex/ex'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'ex t.in',
                             prefix=('env: EXINIT=set directory=.',))
