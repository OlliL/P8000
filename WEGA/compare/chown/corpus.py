"""Corpus of chown (host/recon/corpusrun.py): the original /bin/chown against the build (src/cmd/chown, chown.c of System III with
the messages of WEGA (printf on the standard output) and the WEGA sccsid, ccv1, libc-v6.a), on the cases of
compare/chown/tests.txt (host/recon/casefile.py): numbers and names of /etc/passwd of the root, the messages of the source.
Without user: the guest has the ids of the host user; the cases with user: run with the
owner model of z8k-emu (Z8K_UID), where "Not owner" and a missing file give the perror of the source."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'chown'
ORIG = 'bin/chown'
PROG = 'chown'
CANDIDATES = {'wega': 'src/cmd/chown'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'chown')
