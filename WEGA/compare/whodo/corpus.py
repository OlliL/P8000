"""Corpus of whodo (host/recon/corpusrun.py): the original sh script /bin/whodo against src/cmd/whodo.sh, at its place in the tree
(INSTALLED), on the cases of compare/whodo/tests.txt. whodo puts the output of who (utmp, made by host/tools/mkutmp.py in the layer) and
of the original /bin/ps -a together. ps reads the process table that host/tools/mkproc.py makes (the images of /dev/kmem, /dev/mem and
/dev/swap of the case: the model of the process table of the kernel). The console is the terminal of the user me."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'whodo'
ORIG = 'bin/whodo'
PROG = 'whodo'
CANDIDATES = {'wega': 'src/cmd/whodo.sh'}
PIDS = True
INSTALLED = True
TIMEOUT = 120
PRE = ['user: 0 0', 'time: 600000000', 'terminal: console', 'device: kmem image:km.img', 'device: mem image:mem.img',
       'device: swap image:swap.img', 'host: mkdir -p $Z8K_OVERLAY/etc $Z8K_OVERLAY/tmp']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/bin/whodo', prefix=PRE)
