"""Configuration of the recon toolkit (host/recon/progcfg.py) for popt."""
import os

NAME = 'popt'
ORIG = 'lib/popt'
OWN_LIM = 0x4422   # the program part of /lib/popt ends here (markregs, the last function of z23.c); the libc follows
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|bss|z20|z21|z22|z23|crt0-popt)'

# the model of the old compiler v0p (host/OLDCC.md): the sources compiled with ccv0p (CCHOST), the data in the layout
# of the old cc (d12model.py), linked like `make popt`; strictcmp compares it with /lib/popt
MODULES = ['z20', 'z21', 'z22', 'z23']
TABLES = []
D18 = True          # the old cc writes 0(fp) for @fp (v0, OLD_D18)
D6TABLES = True     # the switch tables in the layout of the old cc (d12model.py d6tables)


def model_link(work, objs):
    """the link of d12model.py: crt0.o, our modules, the libc of the program"""
    here = os.path.dirname(os.path.abspath(__file__))
    src = os.path.normpath(os.path.join(here, '..'))
    hostld = os.path.normpath(os.path.join(src, '..', '..', '..', 'host', 'ld', 'ld'))
    libc = [os.path.join(src, '..', '..', 'lib', 'libc-v5.a')]
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', os.path.join(os.environ.get('WEGA', '/mnt/share/cvs/P8000/tmp/wega31_root'), 'lib', 'crt0.o')] + objs + libc + \
        ['-o', 'poptm']


MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_popt', 'poptm')
STRICT_INPUT = MODEL_OUT
STATICFUNCS = True  # the static functions get global labels in the model (d12model.py), so that strictcmp finds them
