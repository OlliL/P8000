"""Corpus of mfs, umfs (host/recon/corpusrun.py): the original script /etc/mfs (and its other name /etc/umfs) against
src/cmd/mfs.sh, at its place in the tree (INSTALLED: the layer over ROOT of each case), on the cases of
compare/mfs/tests.txt (host/recon/casefile.py). The script runs the original /etc/mount and /etc/umount of the tree;
the partitions /dev/z, /dev/tmp, /dev/usr are images of the case with a WEGA file system (host/tools/wegafs.py), mounted
read only by the model of z8k-emu (Z8K_MOUNTS). /etc/mnttab, which mount and umount change, is copied for every case
(ROOT_COPY)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mfs'
ORIG = 'etc/mfs'
PROG = 'mfs'
CANDIDATES = {'wega': 'src/cmd/mfs.sh'}
ALIASES = {'umfs': 'etc/umfs'}
INSTALLED = True
ROOT_COPY = ['etc/mnttab']
TIMEOUT = 60
FS = 'python3 $WEGA_HOST/tools/wegafs.py'
PRE = ['user: 0 0', 'time: 600000000', 'device: z image:z.img', 'device: tmp image:tmp.img', 'device: usr image:usr.img',
       "host: printf 'in z\\n' > m1 && printf 'in usr\\n' > m2 && " + ' && '.join(
           ['%s format %s.img 400 600000000' % (FS, d) for d in ('z', 'tmp', 'usr')] +
           ['%s put z.img /zfile m1 644 3 3 600000000' % FS, '%s mkdir usr.img /lib 755 3 3 600000000' % FS,
            '%s put usr.img /lib/ufile m2 644 3 3 600000000' % FS]) + ' && rm m1 m2']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/mfs', prefix=PRE)
