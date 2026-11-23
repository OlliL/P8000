"""Corpus of find (host/recon/corpusrun.py): the original /bin/find against the build (src/cmd/find, find.c of System III
with the WEGA changes: chdir(strlen(Pathname)? Pathname: "/"), the globals in the order of the original, the label in
scomp; ccv0p, libc-v7.a), on the cases of compare/find/tests.txt (host/recon/casefile.py): the predicates of the source
(-name -type -size -links -perm -user -group -inum -newer -mtime -atime -ctime -print -exec -o ! ( ) and scomp's + and -),
the walk of a tree, the messages. The clock is fixed (time:); the times of the files are those of the case. Not in the
corpus: -ok and -cpio (a terminal, a device)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'find'
ORIG = 'bin/find'
PROG = 'find'
CANDIDATES = {'wega': 'src/cmd/find'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'find')
