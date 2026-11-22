"""Corpus of env (host/recon/corpusrun.py): the original /bin/env against the build (src/cmd/env, env.c of System III with
the WEGA sccsid, ccv0, libc-v1.a with execvp, getenv, index, strchr and the errlst of 07/82), on the cases of
compare/env/tests.txt (host/recon/casefile.py): print, "-", NAME=VALUE, a command (found by execvp in PATH), the
messages of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'env'
ORIG = 'bin/env'
PROG = 'env'
CANDIDATES = {'wega': 'src/cmd/env'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'env')
