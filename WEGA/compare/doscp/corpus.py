"""Corpus of doscp (host/recon/corpusrun.py): the original /usr/bin/doscp against the shipped build (doscopy version
1.0, src/cmd/doscopy/Makefile), on the cases of compare/doscp/tests.txt (host/recon/casefile.py). The proof that the
floppy model carries the dos programs (increment 117): they read and write /dev/rfd* (image files of the case);
dosformat runs /bin/format and with it the 8-bit part behind /dev/ud. The other dos programs run as the originals of the
tree (by their paths); their own corpora are increment 120."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'doscp'
ORIG = 'usr/bin/doscp'
PROG = 'doscp'
CANDIDATES = {'wega': 'src/cmd/doscopy/1.0/doscp'}
TIMEOUT = 60
TOUCH = "host: find . -type f -exec touch -d '1989-01-05 05:39:00 UTC' {} +"	# doscp takes the time of the WEGA file
PRE = ['time: 600000000', 'terminal: console', 'device: drive1 d1', 'device: ud', TOUCH,
       "host: python3 -c \"open('d1','wb').write(bytes([0xe5]) * 737280)\"",
       'step: /usr/bin/dosformat /dev/rfd1', 'stdin: y']


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'doscp', prefix=PRE)
