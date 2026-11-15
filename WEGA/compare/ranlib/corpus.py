"""Corpus of ranlib (host/recon/corpusrun.py): the command lines of compare/binutils/tree.py on the corpus tree made there
(objects, executables and libraries of the WEGA tree, byte swapped, damaged and odd files, archives); the original against the
shipped build and the host build."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'binutils'))
import tree  # noqa: E402

NAME = 'ranlib'
ORIG = 'usr/bin/ranlib'
PROG = 'ranlib'
CANDIDATES = {'wega': 'src/cmd/ranlib', 'host': 'host/binutils/ranlib'}
ROOT_FILES = ['bin/ar', 'bin/sh']


def generate(g):
    return tree.generate(g, PROG, None)


DEPENDS = {'host': ['host/ar/ar']}	# the host ranlib runs the host ar


def HOST_ENV(root):
    """the host ranlib lets the host ar insert the table (RANLIB_AR)"""
    return {'RANLIB_AR': os.path.normpath(os.path.join(HERE, '..', '..', 'host', 'ar', 'ar'))}


def normalize(what, data):
    """ranlib stores the time of the run: the date of the first member (__.SYMDEF) of an archive is cleared"""
    if isinstance(data, str) or not what.endswith('.a'):
        return data
    if data[:2] == b'\xff\x65' and data[2:11] == b'__.SYMDEF':
        b = bytearray(data)
        b[16:20] = b'\0\0\0\0'
        return bytes(b)
    return data
