"""Configuration of the recon toolkit (host/recon/progcfg.py) for sld (segmented, built with the shipped scc)."""
import atexit
import importlib.util
import json
import os
import shlex
import shutil
import tempfile
import threading

NAME = 'sld'
ORIG = 'bin/sld'
OWN_LIM = 0x71ca    # the own code of /bin/sld ends here; the members of slibc-v9.a follow
ORIG_LIM = 0x10000  # the original is stripped
SEG = True
CC = '/bin/scc'
MODULES = ['copyname', 'dump', 'ext', 'ld', 'ldm', 'printname', 'swap', 'symbols']   # link order of the own modules

REPO_ = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


