"""Corpus of sort (host/recon/corpusrun.py): the original /bin/sort against the build (src/cmd/sort, sort.c of System III
with the WEGA order of the temporary directories (/tmp first) and the sccsid, ccv1, libc-v12.a), on the cases of
compare/sort/tests.txt (host/recon/casefile.py): the options and keys of the source, -c, -m, -o, -u and the messages."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'sort'
ORIG = 'bin/sort'
PROG = 'sort'
CANDIDATES = {'wega': 'src/cmd/sort'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'sort t.in')
