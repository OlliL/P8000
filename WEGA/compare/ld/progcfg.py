"""Configuration of the recon toolkit (host/recon/progcfg.py) for ld (nonsegmented, stripped)."""
import atexit
import importlib.util
import json
import os
import shlex
import shutil
import tempfile
import threading

NAME = 'ld'
ORIG = 'bin/ld'
OWN_LIM = 0x2c4c    # the own code of /bin/ld ends here; shipped libc members follow (libcver.py)
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|copyname|printname|symbols|ld)'
CC = '/bin/cc'
MODULES = ['ld', 'copyname', 'printname', 'symbols']   # link order of the own modules
TABLES = []

REPO_ = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))


def model_link(work, objs):
    """the link of d12model.py: crt0, our modules, the libc of the program, like `make ld`"""
    here = os.path.dirname(os.path.abspath(__file__))
    ld = os.path.normpath(os.path.join(here, '..', '..', 'src', 'cmd', 'ld'))
    libc = [os.path.join(ld, '..', '..', 'lib', 'libc-v9.a')]
    hostld = os.path.normpath(os.path.join(ld, '..', '..', '..', 'host', 'ld', 'ld'))
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', os.path.join(ld, 'crt0.o')] + objs + libc + ['-o', 'ldm']


# the strict comparison runs on the d12model link (our data layout differs from the original only by D12)
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/home/olivleh1/git/P8000/tmp/tmp'), 'd12model_ld', 'ldm')
STRICT_INPUT = MODEL_OUT

