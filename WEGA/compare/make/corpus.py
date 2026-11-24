"""Corpus of make (host/recon/corpusrun.py): the original /bin/make against the build. The cases are the blocks of
compare/make/tests.txt (host/recon/casefile.py: the format; the default file is makefile, the default step make): the man
page make(1) of WEGA (the options, the description file, macros, suffix rules, .SILENT .IGNORE .DEFAULT .PRECIOUS, the
interrupt) and what the source has besides (the messages, the exit status, the WEGA parts: the chkout tree of get(), the
regular file of intrupt, csh as SHELL, the entry point of an object archive by symscan). Every case has an expectation on
the original, written from the man page or the source, not taken from a run. The times of the files that make compares are
set by touch in host lines.
Not in the corpus: $(MAKE) (the sub-make is /bin/make of the tree on both sides), -m (the memory map), -d (addresses)."""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
NAME = 'make'
ORIG = 'bin/make'
PROG = 'make'
CANDIDATES = {'wega': 'src/cmd/make/make'}
TESTS = os.path.join(HERE, 'tests.txt')
TIMEOUT = 30


def after_step(argv, rc, out, err, rd):
    """the message of intrupt() (stderr, unbuffered) comes in pieces whose line breaks change with the time of the
    interrupt: its newlines are not compared"""
    if 'removed' in err.replace('\n', ''):
        err = err.replace('\n', '')
    return out, err


def generate(g):
    return casefile.generate(g, TESTS, 'make', first='makefile')
