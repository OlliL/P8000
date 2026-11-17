"""Corpus of new.install (host/recon/corpusrun.py): the original csh script /etc/new.install against src/cmd/new.install.sh, at its place in
the tree (INSTALLED), on the cases of compare/newinstall/tests.txt. The file systems are images of the case, as raw (rmd0 /dev/rusr,
rmd2 /dev/rroot, rmd3 /dev/rtmp, rmd4 /dev/rz) and as block devices (usr, tmp, z) for /etc/mfs, which mounts them (read only: the
model of z8k-emu); the script makes them with the original mkfs, names them with labelit, writes the boot block to /dev/rmd0 (the
image of /usr) and makes lost+found, which are directories of the layer over the tree (/ and /usr), so that nothing is written into
the tree. The man pages of man0 to man5 are symbolic links in the layer, a case packs some with the original pack, and unpack of
the script unpacks them. What makelost+found writes into the mounted /tmp and /z is not kept (compare/README.md, section 12)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'newinstall'
ORIG = 'etc/new.install'
PROG = 'new.install'
CANDIDATES = {'wega': 'src/cmd/new.install.sh'}
INSTALLED = True
ROOT_COPY = ['etc/mnttab']
TIMEOUT = 600
FS = 'python3 $WEGA_HOST/tools/wegafs.py'
LINKS = ('for d in man0 man1 man2 man3 man4 man5; do mkdir -p $Z8K_OVERLAY/usr/man/$d; '
         'ln -s $WEGA_ROOT/usr/man/$d/* $Z8K_OVERLAY/usr/man/$d/; done')
PRE = ['user: 0 0', 'time: 600000000',
       'device: rmd0 image:usr.img', 'device: rmd2 image:root.img', 'device: rmd3 image:tmp.img', 'device: rmd4 image:z.img',
       'device: usr image:usr.img', 'device: tmp image:tmp.img', 'device: z image:z.img', 'device: root image:root.img',
       'host: ' + '; '.join(['%s format %s.img 400 500000000' % (FS, d) for d in ('usr', 'root', 'tmp', 'z')]) +
       '; mkdir -p $Z8K_OVERLAY/lost+found $Z8K_OVERLAY/usr/lost+found $Z8K_OVERLAY/tmp; ' + LINKS]


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/new.install', prefix=PRE)
