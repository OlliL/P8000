"""Corpus of mv, cp and ln (host/recon/corpusrun.py): one program, src/cmd/mv.c, installed as /bin/mv, /bin/cp and
/bin/ln (links); it chooses its work by the last part of argv[0]. The original under each name against the shipped
build under the same name (ALIASES; argv[0] is the word of the step, z8k-emu -0), on the cases of compare/mv/tests.txt
(host/recon/casefile.py): the man pages of the three and every message of the source; the name "copy" runs /bin/mv as
another name. Not in the corpus: the question before a target without write permission (needs a terminal), EXDEV (one
file system), names longer than the buffers of 128."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'mv'
ORIG = 'bin/mv'
PROG = 'mv'
ALIASES = {'cp': 'bin/cp', 'ln': 'bin/ln', 'copy': 'bin/mv'}
CANDIDATES = {'wega': 'src/cmd/mv'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'mv t.in')
