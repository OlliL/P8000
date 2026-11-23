"""Corpus of su (host/recon/corpusrun.py): the original /bin/su against the build (src/cmd/su, su.c of System III with the
WEGA changes: "wega" as the default id, TERM passed on in the environment of "su -", rindex, the sccsid; ccv1, libc-v4.a),
on the cases of compare/su/tests.txt (host/recon/casefile.py): as root (no password asked) with a command, "-", an
unknown id; as a user the password is asked on /dev/tty (the console model). The log /usr/adm/sulog goes to the layer of
the case (every case makes its directory there first; without it the log
would be written to ROOT)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'su'
ORIG = 'bin/su'
PROG = 'su'
CANDIDATES = {'wega': 'src/cmd/su'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'su')
