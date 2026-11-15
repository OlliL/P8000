"""Corpus of size (host/recon/corpusrun.py): the command lines of compare/binutils/tree.py on the corpus tree made there
(objects, executables and libraries of the WEGA tree, byte swapped, damaged and odd files, archives); the original against the
shipped build and the host build."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'binutils'))
import tree  # noqa: E402

NAME = 'size'
ORIG = 'bin/size'
PROG = 'size'
CANDIDATES = {'wega': 'src/cmd/size', 'host': 'host/binutils/size'}
ROOT_FILES = ['bin/ar', 'bin/sh']


def generate(g):
    return tree.generate(g, PROG, {0: 'expect: exit 0\n'})
