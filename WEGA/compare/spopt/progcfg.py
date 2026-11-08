"""Configuration of the recon toolkit (host/recon/progcfg.py) for spopt: the sources of popt with -DSPOPT."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_s = importlib.util.spec_from_file_location('popt_progcfg', os.path.join(HERE, '..', 'popt', 'progcfg.py'))
_p = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_p)

NAME = 'spopt'
ORIG = 'lib/spopt'
OWN_LIM = 0x453c    # the program part of /lib/spopt ends here (markregs); the libc follows
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = _p.OWN_OBJ
SRCDIR = os.path.normpath(os.path.join(HERE, '..', '..', 'src', 'cmd', 'popt'))
MODULES = _p.MODULES
TABLES = []
D18 = True
D6TABLES = True
CCFLAGS = ['-DSPOPT']
model_link = _p.model_link
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_spopt', 'poptm')
STRICT_INPUT = MODEL_OUT
STATICFUNCS = True


# the corpus: every C source; the input from the original /bin/scc -S (no -O), each run without option, with -l and -d.
# The line "NK core" of -d may differ: the place of the heap of a build with another libc (OLDCC_LOG.md)
CANDIDATES = {'wega': 'src/cmd/popt/build/seg/spopt', 'host': 'host/popt/spopt'}
CORPUS_VARIANTS = [[], ['-l'], ['-d']]
CORPUS_IGNORE = rb'\d+K core'


def CORPUS_SOURCES():
    import glob
    import corpusc
    return corpusc.c_sources() + sorted(glob.glob(os.path.join(HERE, '..', 'popt', 'cases', '*.c')))


def CORPUS_PREPARE(src, work):
    import corpusc
    return corpusc.prepare_s(src, work, '/bin/scc')


def CORPUS_RUN(binary, base, work, variant):
    return [binary] + variant + [base + '.s', 'out.s'], None


def CORPUS_OUTPUTS(base):
    return ['out.s']
