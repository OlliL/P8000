"""Corpus of killall (host/recon/corpusrun.py): the original sh script /etc/killall against src/cmd/killall.sh, at its place in the tree
(INSTALLED), on the cases of compare/killall/tests.txt. killall takes the processes from the original /bin/ps -ef and his own terminal
from who am i (utmp in the layer) and kills the others. ps reads the process table that host/tools/mkproc.py makes (the images of
/dev/kmem, /dev/mem and /dev/swap of the case: the model of the process table of the kernel). The processes of the model are no host
processes: a kill of their numbers is written to $Z8K_KILLLOG (z8k-emu) and granted, the case reads it with cat."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'killall'
ORIG = 'etc/killall'
PROG = 'killall'
CANDIDATES = {'wega': 'src/cmd/killall.sh'}
PIDS = True
INSTALLED = True
TIMEOUT = 120
PRE = ['user: 0 0', 'time: 600000000', 'terminal: console', 'device: kmem image:km.img', 'device: mem image:mem.img',
       'device: swap image:swap.img',
       'host: mkdir -p $Z8K_OVERLAY/etc $Z8K_OVERLAY/tmp && python3 $WEGA_HOST/tools/mkutmp.py $Z8K_OVERLAY/etc/utmp console:me tty1:bob tty2:amy']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/killall', prefix=PRE)
