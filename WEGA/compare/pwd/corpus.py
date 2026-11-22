"""Corpus of pwd (host/recon/corpusrun.py): the original /bin/pwd against the build (src/cmd/pwd, pwd.c of System III with
the WEGA sccsid and the order of its globals, ccv0, libc-v1.a), on the cases of compare/pwd/tests.txt
(host/recon/casefile.py): directories below the root of the emulator the directory of a case (outside the root of the emulator the guest
sees the path of the host; after_step writes the directory of the case as CASEDIR). Not in the corpus: the messages
(a directory without permission or a removed one)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'pwd'
ORIG = 'bin/pwd'
PROG = 'pwd'
CANDIDATES = {'wega': 'src/cmd/pwd'}
TIMEOUT = 60


def after_step(argv, rc, out, err, rd):
    """the directory of the case depends on the run"""
    for d in (os.path.realpath(rd), rd):
        out = out.replace(d, 'CASEDIR')
    return out, err


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'pwd')
