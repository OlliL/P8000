"""Configuration of the recon toolkit (host/recon/progcfg.py) for cpp (/lib/cpp), built with ccv0p."""
import os

NAME = 'cpp'
ORIG = 'lib/cpp'
OWN_LIM = 0x2272    # the own code ends here (the last ret of yyparse); fopen of the libc follows
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|cpp|cpy)'
MODULES = ['cpp', 'cpy']
TABLES = []
D18 = True
D6TABLES = True
STATICFUNCS = True


def model_link(work, objs):
    """the link of d12model.py: crt0.o (make crt0.o), our modules, the libc part of the Makefile"""
    import re
    here = os.path.dirname(os.path.abspath(__file__))
    src = os.path.normpath(os.path.join(here, '..'))
    hostld = os.path.normpath(os.path.join(src, '..', '..', '..', 'host', 'ld', 'ld'))
    mk = open(os.path.join(src, 'Makefile')).read().replace('\\\n', ' ')
    libc = re.search(r'^LIBCOBJS = (.*)$', mk, re.M).group(1).split()
    libc = [os.path.join(src, o.replace('$(OLD)', os.path.join('..', '..', 'lib', 'oldlibc'))) for o in libc]
    return [os.environ.get('LDBIN', hostld), '-e', 'start', os.path.join(src, 'crt0.o')] + objs + libc + ['-o', 'cppm']


MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cpp', 'cppm')
STRICT_INPUT = MODEL_OUT
