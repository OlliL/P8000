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
    """the link of d12model.py: start-up file, our modules, the libc part"""
    here = os.path.dirname(os.path.abspath(__file__))
    src = os.path.normpath(os.path.join(here, '..'))
    hostld = os.path.normpath(os.path.join(src, '..', '..', '..', 'host', 'ld', 'ld'))
    root = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', os.path.join(root, 'lib', 'crt0.o')] + objs + \
        [os.path.join(root, 'lib', 'libc.a'), '-o', 'cppm']


MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cpp', 'cppm')
STRICT_INPUT = MODEL_OUT
