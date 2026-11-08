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

CORPUS_BINARY = 'ld'
CANDIDATES = {'wega': 'src/cmd/ld/ld', 'host': 'host/ld/ld'}
REPO_ = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
CORPUS_FIXTURES = None
CORPUS_LOCK = threading.Lock()


def _ld_check():
    path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'fixtures.py')
    spec = importlib.util.spec_from_file_location('ld_check_corpus', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _setup_corpus():
    global CORPUS_FIXTURES
    with CORPUS_LOCK:
        if CORPUS_FIXTURES is not None:
            return
    check = _ld_check()
    if not os.path.isdir(ROOT):
        raise FileNotFoundError('set WEGA to the file tree of the original system')
    check.ROOT = ROOT
    check.EMU = EMU
    with CORPUS_LOCK:
        if CORPUS_FIXTURES is None:
            CORPUS_FIXTURES = tempfile.mkdtemp(prefix='corpus_ld_fixtures_')
            atexit.register(shutil.rmtree, CORPUS_FIXTURES, ignore_errors=True)
            check.W = CORPUS_FIXTURES
            check.setup()


def CORPUS_SOURCES():
    """the fixed and random command lines of fixtures.py"""
    _setup_corpus()
    check = _ld_check()
    return check.FIXED + [' '.join(args) for args in check.rnd(100, 1)]


def CORPUS_PREPARE(src, work):
    """copy the linker's test objects and libraries and save this command line for both runs."""
    _setup_corpus()
    shutil.copytree(os.path.join(CORPUS_FIXTURES, 'base'), work, dirs_exist_ok=True)
    args = shlex.split(src)
    output = 'a.out'
    for i, arg in enumerate(args):
        if arg == '-o' and i + 1 < len(args):
            output = args[i + 1]
    with open(os.path.join(work, 'corpus_args.json'), 'w') as f:
        json.dump(args, f)
    return output


def CORPUS_RUN(binary, base, work):
    with open(os.path.join(work, 'corpus_args.json')) as f:
        args = json.load(f)
    return [binary] + args, None


def CORPUS_OUTPUTS(base):
    return [base]


def model_link(work, objs):
    """the link of d12model.py: crt0, our modules, the libc of the program, like `make ld`"""
    here = os.path.dirname(os.path.abspath(__file__))
    ld = os.path.normpath(os.path.join(here, '..', '..', 'src', 'cmd', 'ld'))
    libc = [os.path.join(ld, '..', '..', 'lib', 'libc-v9.a')]
    hostld = os.path.normpath(os.path.join(ld, '..', '..', '..', 'host', 'ld', 'ld'))
    return [os.environ.get('LDBIN', hostld), '-i', '-e', 'start', os.path.join(ld, 'crt0.o')] + objs + libc + ['-o', 'ldm']


# the strict comparison runs on the d12model link (our data layout differs from the original only by D12)
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_ld', 'ldm')
STRICT_INPUT = MODEL_OUT

# files of the tree the runs read (part of the cache key; host/tools/corpusdeps.sh checks the list)
CORPUS_ROOT_FILES = ['lib/crt0.o', 'lib/libc.a']
