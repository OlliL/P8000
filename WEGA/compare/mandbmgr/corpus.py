"""Corpus of man.dbmgr (host/recon/corpusrun.py): the original csh script /etc/man.dbmgr against src/cmd/man.dbmgr.sh, at its
place in the tree (INSTALLED), on the cases of compare/mandbmgr/tests.txt. /dev/tmp is an image of the case with a WEGA file system
(host/tools/wegafs.py: its number of free blocks decides the largest size of the database); /dev/kmem is the minimal image that
df -f needs (host/tools/mkkmem.py); /tmp is a directory of the layer (the whole guest /tmp: the tmp/man of the tree is not seen)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mandbmgr'
ORIG = 'etc/man.dbmgr'
PROG = 'man.dbmgr'
CANDIDATES = {'wega': 'src/cmd/man.dbmgr.sh'}
INSTALLED = True
TIMEOUT = 300
FS = 'python3 $WEGA_HOST/tools/wegafs.py format tmp.img %d 500000000'


def prefix(blocks=1200):
    return ['user: 0 0', 'time: 600000000', 'device: tmp image:tmp.img', 'device: kmem image:km.img',
            'host: ' + FS % blocks + '; python3 $WEGA_HOST/tools/mkkmem.py km.img; mkdir -p $Z8K_OVERLAY/tmp']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/man.dbmgr', prefix=prefix())
