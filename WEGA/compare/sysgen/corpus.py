"""Corpus of sysgen (host/recon/corpusrun.py): the original csh script /etc/sysgen against src/cmd/sysgen.sh, at its place in the tree
(INSTALLED), on the cases of compare/sysgen/tests.txt. The script works in /usr/sys/conf of the tree and writes ../h/mdsize.h, ../h/ver.h,
../dev/LIB2 and builds the kernel with make (scc, sld): /usr/sys is a COPY in the layer over the tree (the files are written, a link
would write the tree; their times are made old, objects younger than sources, so that make builds only what a case changes: the
times of the copy would decide otherwise). The kernel and the boot program it makes are copied to the case directory afterwards (wega.out, boot.out): their
bytes are compared, with the headers of /usr/sys/h. vi (the first question) is not answered with y: no screen."""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'sysgen'
ORIG = 'etc/sysgen'
PROG = 'sysgen'
CANDIDATES = {'wega': 'src/cmd/sysgen.sh'}
INSTALLED = True
TIMEOUT = 900
PRE = ['user: 0 0', 'time: 600000000',
       'host: mkdir -p $Z8K_OVERLAY/usr $Z8K_OVERLAY/tmp && cp -r $WEGA_ROOT/usr/sys $Z8K_OVERLAY/usr/sys && chmod -R u+w $Z8K_OVERLAY/usr/sys && find $Z8K_OVERLAY/usr/sys -exec touch -d "1989-01-01" {} + && find $Z8K_OVERLAY/usr/sys -name "*.o" -exec touch -d "1989-01-02" {} +']
def normalize(kind, text):
    """the path in the messages 'New wega : DIR/wega' and 'New boot : DIR/boot': the directory of the layer holds the name of the
    side and is cut by the old pwd after 64 characters"""
    return re.sub(r'(New (?:wega|boot) : )\S*', r'\1DIR', text) if kind == 'output' else text


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/sysgen', prefix=PRE)
