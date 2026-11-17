"""Corpus of adduser (host/recon/corpusrun.py): the original csh script /etc/adduser against src/cmd/adduser.sh, at its place in the tree
(INSTALLED), on the cases of compare/adduser/tests.txt. /etc/passwd, /etc/group and /etc/whois are copies of the tree's files in the layer
(ROOT_COPY), the home directories and the mail files are made in the layer (usr/...); the console is the terminal (passwd asks twice for
the password on it): the answers of the operator, the password at the end, are the standard input. The process numbers of the guest are
fixed (Z8K_PIDDIR), since passwd makes its salt of the process number and the time (time: is fixed)."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'adduser'
ORIG = 'etc/adduser'
PROG = 'adduser'
CANDIDATES = {'wega': 'src/cmd/adduser.sh'}
PIDS = True
INSTALLED = True
ROOT_COPY = ['etc/passwd', 'etc/group', 'etc/whois']
TIMEOUT = 120
PRE = ['user: 0 0', 'time: 600000000', 'terminal: console',
       'host: mkdir -p $Z8K_OVERLAY/tmp $Z8K_OVERLAY/usr/spool/mail $Z8K_OVERLAY/usr/home']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/adduser', prefix=PRE)
