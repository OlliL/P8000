"""Configuration of the recon toolkit (host/recon/progcfg.py) for ld (nonsegmented, stripped)."""
NAME = 'ld'
ORIG = 'bin/ld'
OWN_LIM = 0x2c4c    # the own code of /bin/ld ends here; shipped libc members follow (libcver.py)
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|copyname|printname|symbols|ld)'
CC = '/bin/cc'
MODULES = ['ld', 'copyname', 'printname', 'symbols']   # link order of the own modules
TABLES = ['assigned.txt', 'datamap.txt', 'names.txt', 'bss_map.txt']


def model_link(work, objs):
    """the link of d12model.py: crt0, our modules, the libc part of the Makefile (LIBCOBJS), like `make ld`"""
    import os
    import re
    here = os.path.dirname(os.path.abspath(__file__))
    ld = os.path.normpath(os.path.join(here, '..'))
    mk = open(os.path.join(ld, 'Makefile')).read().replace('\\\n', ' ')
    libc = re.search(r'^LIBCOBJS = (.*)$', mk, re.M).group(1).split()
    libc = [o.replace('$(OLD)', os.path.join(ld, '..', '..', 'lib', 'oldlibc')) for o in libc]
    libc = [o if o.startswith('/') else os.path.join(ld, o) for o in libc]
    hostld = os.path.normpath(os.path.join(ld, '..', '..', '..', 'host', 'ld', 'ld'))
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', os.path.join(ld, 'crt0.o')] + objs + libc + ['-o', 'ldm']


# the strict comparison runs on the d12model link (our data layout differs from the original only by D12)
import os as _os  # noqa: E402
MODEL_OUT = _os.path.join(_os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_ld', 'ldm')
STRICT_INPUT = MODEL_OUT
