"""Corpus of gld (segmented loader of its own, WEGA 3.1 /bin/gld): the command lines of fixtures.py (GFIXED and the random ones of a fixed seed) on the test
objects of the sld corpus and its own (gx1 ... gx4, psec1, archives, damaged objects). There is no source of gld in the tree: the only
candidate is the host build (host/ld/gld), written from the binary."""
import atexit
import importlib.util
import json
import os
import shlex
import shutil
import tempfile
import threading

NAME = 'gld'
ORIG = 'bin/gld'
CANDIDATES = {'host': 'host/ld/gld'}
CORPUS_BINARY = 'gld'
SEG = True
CC = '/bin/scc'

HERE = os.path.dirname(os.path.abspath(__file__))
CORPUS_FIXTURES = None
CORPUS_LOCK = threading.Lock()


def _load(name):
    spec = importlib.util.spec_from_file_location('fixtures_' + name, os.path.join(HERE, '..', name, 'fixtures.py'))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.ROOT = ROOT
    module.EMU = EMU
    return module


def _setup_corpus():
    global CORPUS_FIXTURES
    with CORPUS_LOCK:
        if CORPUS_FIXTURES is None:
            CORPUS_FIXTURES = tempfile.mkdtemp(prefix='corpus_gld_fixtures_')
            atexit.register(shutil.rmtree, CORPUS_FIXTURES, ignore_errors=True)
            sld = _load('sld')
            sld.W = CORPUS_FIXTURES
            sld.setup_seg()		# the objects of sld in sbase
            gld = _load('gld')
            gld.W = CORPUS_FIXTURES
            gld.setup_gld()		# and those of gld


def CORPUS_SOURCES():
    _setup_corpus()
    gld = _load('gld')
    return list(gld.GFIXED) + [' '.join(args) for args in gld.rnd_gld(50, 1)]


def CORPUS_PREPARE(src, work):
    _setup_corpus()
    shutil.copytree(os.path.join(CORPUS_FIXTURES, 'sbase'), work, dirs_exist_ok=True)
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

# files of the tree the runs read (part of the cache key; host/tools/corpusdeps.sh checks the list)
CORPUS_ROOT_FILES = ['lib/scrt0.o', 'lib/slibc.a', 'lib/slibib.a', 'usr/lib/slibib.a', 'z/bin/lib/slibib.a']
