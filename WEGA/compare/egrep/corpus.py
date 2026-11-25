"""Corpus of egrep (host/recon/corpusrun.py): the original /bin/egrep against the build (src/cmd/egrep, egrep.y of System III by yacc,
with the WEGA sccsid and the -e test of the usage; ccvm and libc.a), on the cases of compare/egrep/tests.txt
(host/recon/casefile.py): every option of the source (-b -c -e -f -l -n -v), several files, the standard input, the
regular expressions of the grammar (| + ? * ( ) [ ] . ^ $, the newline) and the messages."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'egrep'
ORIG = 'bin/egrep'
PROG = 'egrep'
CANDIDATES = {'wega': 'src/cmd/egrep'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'egrep apple t.in')
