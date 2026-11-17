"""Corpus of makewhatis (host/recon/corpusrun.py): the original csh script /etc/makewhatis against src/cmd/makewhatis.sh, at its place
in the tree (INSTALLED), on the cases of compare/makewhatis/tests.txt. The script works in /usr/man (a temporary file in each of
the directories man1 ... man7, manM) and /tmp and writes /usr/lib/whatis: all of them are directories of the layer over the tree;
the man directories of the layer hold symbolic links to the pages of the tree (a case can change one of them: a copy)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'makewhatis'
ORIG = 'etc/makewhatis'
PROG = 'makewhatis'
CANDIDATES = {'wega': 'src/cmd/makewhatis.sh'}
INSTALLED = True
ROOT_COPY = ['usr/lib/whatis']
TIMEOUT = 600
LINKS = ('for d in man1 man2 man3 man4 man5 man6 man7 manM; do mkdir -p $Z8K_OVERLAY/usr/man/$d; '
         'ln -s $WEGA_ROOT/usr/man/$d/* $Z8K_OVERLAY/usr/man/$d/; done; mkdir -p $Z8K_OVERLAY/tmp')


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/makewhatis', prefix=['user: 0 0', 'time: 600000000', 'host: ' + LINKS])
