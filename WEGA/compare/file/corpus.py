"""Corpus of file (host/recon/corpusrun.py): the original /bin/file against the build (src/cmd/file, file.c of System III
rewritten by WEGA, ccv0 and libc-v1), on the cases of compare/file/tests.txt (host/recon/casefile.py): every type it
tells (the s.out formats, also Z8/Z80 and byte swapped, archives, C, csh, fortran, assembler, roff, Z8, plz/asm, plz/sys,
English, commands, data, troff output), -f, files of the WEGA root and the messages."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'file'
ORIG = 'bin/file'
PROG = 'file'
CANDIDATES = {'wega': 'src/cmd/file'}
TIMEOUT = 60
RC_UNDEFINED = True    # main ends without exit (without -f): the status is what r2 holds


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'file t.in')
