"""Corpus of cpio (host/recon/corpusrun.py): the original usr/bin/cpio against the shipped build, on the cases of
compare/cpio/tests.txt (host/recon/casefile.py): the man page (cpio.1) and every message of the source."""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import casefile  # noqa: E402

NAME = 'cpio'
ORIG = 'usr/bin/cpio'
PROG = 'cpio'
CANDIDATES = {'wega': 'src/cmd/cpio'}
TIMEOUT = 60


def generate(g):
    return casefile.generate(g, os.path.join(HERE, 'tests.txt'), 'cpio t.in')


def normalize(what, data):
    """the device and inode numbers in the headers of an archive on the output are those of the host's files"""
    if what == 'output' and isinstance(data, str):
        data = re.sub(r'( 70707).{12}', r'\g<1>' + '0' * 12, data, flags=re.S)
        data = re.sub('\x71\xc7..(..)'.replace('(..)', '..'), '\x71\xc7\0\0\0\0', data, flags=re.S)
    return data
