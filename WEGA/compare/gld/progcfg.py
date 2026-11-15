"""Corpus of gld (segmented loader of its own, WEGA 3.1 /bin/gld): the command lines of fixtures.py (GFIXED and the random ones of a fixed seed) on the test
objects of the sld corpus and its own (gx1 ... gx4, psec1, archives, damaged objects). There is no source of gld in the tree: the only
candidate is the host build (host/ld/gld), written from the binary."""
import atexit
import importlib.util
import json
import os
import shlex
import shutil
import tempfile
import threading

NAME = 'gld'
ORIG = 'bin/gld'
SEG = True
CC = '/bin/scc'

HERE = os.path.dirname(os.path.abspath(__file__))


