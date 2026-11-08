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
COMBINED = True    # code and data together (strictcmp.py)
D6TABLES = True
STATICFUNCS = True


def model_link(work, objs):
    """the link of d12model.py: crt0.o (make crt0.o), our modules, the libc of the program"""
    here = os.path.dirname(os.path.abspath(__file__))
    src = os.path.normpath(os.path.join(here, '..', '..', 'src', 'cmd', 'cpp'))
    hostld = os.path.normpath(os.path.join(src, '..', '..', '..', 'host', 'ld', 'ld'))
    libc = [os.path.join(src, '..', '..', 'lib', 'libc-v2.a')]
    return [os.environ.get('LDBIN', hostld), '-e', 'start', os.path.join(src, 'crt0.o')] + objs + libc + ['-o', 'cppm']


MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_cpp', 'cppm')
STRICT_INPUT = MODEL_OUT

CORPUS_BINARY = 'cpp'
CANDIDATES = {'wega': 'src/cmd/cpp/cpp', 'host': 'host/cpp/cpp'}


def CORPUS_SOURCES():
    import corpusc
    return corpusc.c_sources()


def CORPUS_PREPARE(src, work):
    import corpusc
    return corpusc.prepare_cpp(src, work)


def CORPUS_RUN(binary, base, work):
    return [binary, '-I.', base, 't.i'], None


def CORPUS_OUTPUTS(base):
    return ['t.i']


def CORPUS_HOST_ENV(root):
    """the host cpp (host/cpp) reads the include directory of the system from CPP_INCLUDE"""
    return {'CPP_INCLUDE': os.path.join(root, 'usr', 'include')}


def CORPUS_NORMALIZE(name, data):
    """the host cpp names the include directory of the system as it was given (CPP_INCLUDE), the original as /usr/include"""
    import corpusc
    return data.replace(os.path.join(corpusc.root(), 'usr', 'include').encode(), b'/usr/include')
