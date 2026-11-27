"""Corpus of rmuser (host/recon/corpusrun.py): the original sh script /etc/rmuser against src/cmd/rmuser.sh, at its place in the tree
(INSTALLED), on the cases of compare/rmuser/tests.txt. /etc/passwd, /etc/group and /etc/whois are copies of the tree's files in the layer
(ROOT_COPY); the case adds a user zed to them and makes his mail file in the layer; the console is the terminal (passwd asks twice for a
password: the standard input). The script lists the files of the user with find / (the whole tree) into files.NAME of the case directory."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'rmuser'
ORIG = 'etc/rmuser'
PROG = 'rmuser'
CANDIDATES = {'wega': 'src/cmd/rmuser.sh'}
PIDS = True
INSTALLED = True
ROOT_COPY = ['etc/passwd', 'etc/group', 'etc/whois']
TIMEOUT = 300
PRE = ['user: 0 0', 'time: 600000000', 'terminal: console',
       'host: mkdir -p $Z8K_OVERLAY/tmp $Z8K_OVERLAY/usr/spool/mail && cd $Z8K_OVERLAY/etc && echo "zed:x:50:1::/usr/zed:/bin/csh" >> passwd '
       '&& echo "zed:::::" >> whois && sed "s/^adm::4:/adm::4:zed,bin/;s/^other::1:/other::1:bin,zed,adm/;s/^system::0:/system::0:zed/" group > group.new && cat group.new > group && rm group.new '
       '&& echo "mail for zed" > $Z8K_OVERLAY/usr/spool/mail/zed']


def after_step(argv, rc, out, err, rd):
    """the diagnostics of find (find: cannot read ..., bad status-- ...) about the walk through the whole tree: which entries it lists
    first depends on the layer (the script itself is in its /etc on one side, in the tree's on the other)"""
    return out, ''.join(l for l in err.splitlines(True) if not l.startswith('find: '))


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), '/etc/rmuser', prefix=PRE)
