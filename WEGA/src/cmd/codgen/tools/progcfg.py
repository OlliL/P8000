"""Configuration of the recon toolkit (host/recon/progcfg.py) for codgen."""
import os

NAME = 'codgen'
ORIG = 'lib/codgen'
OWN_LIM = 0xbbc4   # the program part of the original ends here (the C library follows)
OWN_OBJ = (r'^(crt0|bss3|reader3|rdr3|local3|order3|match3|allo3|regallo3|comm3|table3_c|wrt3|regs3|lcl3|tr3|optim3)')
ORIG_LIM = 0x10000  # the original is stripped

# the model of the old compiler v0p (host/OLDCC.md): the sources compiled with ccv0p (CCHOST), the data in the layout
# of the old cc (d12model.py), linked like `make codgen`; strictcmp compares it with /lib/codgen
MODULES = ['bss3', 'reader3', 'rdr3', 'local3', 'order3', 'match3', 'allo3', 'regallo3', 'comm3', 'table3_codgen', 'wrt3', 'regs3', 'lcl3', 'tr3', 'optim3']
TABLES = []
D18 = True          # the old cc writes 0(fp) for @fp (v0, OLD_D18)
D6TABLES = True     # the switch tables in the layout of the old cc (d12model.py d6tables)
STATICFUNCS = True  # the static functions get global labels in the model (d12model.py), so that strictcmp finds them


def model_link(work, objs):
    """the link of d12model.py: the start-up file, our modules, the libc part of the Makefile (LIBCOBJS)"""
    import re
    here = os.path.dirname(os.path.abspath(__file__))
    src = os.path.normpath(os.path.join(here, '..'))
    hostld = os.path.normpath(os.path.join(src, '..', '..', '..', 'host', 'ld', 'ld'))
    mk = open(os.path.join(src, 'Makefile')).read().replace('\\\n', ' ')

    libc = re.search(r'^LIBCOBJS = (.*)$', mk, re.M).group(1).split()
    libc = [os.path.join(src, o.replace('$(OLD)', os.path.join('..', '..', 'lib', 'oldlibc'))) for o in libc]
    crt = os.path.join(src, '..', '..', 'lib', 'libc', 'crt', 'crt0-codgen.o')
    return [os.environ.get('LDBIN', hostld), '-X', '-e', 'start', '-i', crt] + objs + libc + ['-o', 'codgenm']


MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_codgen', 'codgenm')
STRICT_INPUT = MODEL_OUT
