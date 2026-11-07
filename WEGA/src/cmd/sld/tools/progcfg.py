"""Configuration of the recon toolkit (host/recon/progcfg.py) for sld (segmented, built with the shipped scc)."""
import atexit
import importlib.util
import json
import os
import shlex
import shutil
import tempfile
import threading

NAME = 'sld'
ORIG = 'bin/sld'
OWN_LIM = 0x71ca    # the own code of /bin/sld ends here; the members of slibc-v9.a follow
ORIG_LIM = 0x10000  # the original is stripped
SEG = True
CC = '/bin/scc'
MODULES = ['copyname', 'dump', 'ext', 'ld', 'ldm', 'printname', 'swap', 'symbols']   # link order of the own modules

# the corpus: the segmented command lines of host/ld/check.py (SFIXED and random ones) on its test objects
CORPUS_BINARY = 'sld'
REPO_ = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))
CORPUS_FIXTURES = None
CORPUS_LOCK = threading.Lock()


def _check():
    path = os.path.join(REPO_, 'host', 'ld', 'check.py')
    spec = importlib.util.spec_from_file_location('ld_check_corpus', path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.ROOT = ROOT
    module.EMU = EMU
    return module


def _setup_corpus():
    global CORPUS_FIXTURES
    with CORPUS_LOCK:
        if CORPUS_FIXTURES is None:
            check = _check()
            CORPUS_FIXTURES = tempfile.mkdtemp(prefix='corpus_sld_fixtures_')
            atexit.register(shutil.rmtree, CORPUS_FIXTURES, ignore_errors=True)
            check.W = CORPUS_FIXTURES
            check.setup_seg()


def CORPUS_SOURCES():
    _setup_corpus()
    check = _check()
    kernel = os.path.exists(os.path.join(CORPUS_FIXTURES, 'sbase', 'LIB1'))
    return [l for l in check.SFIXED if kernel or not l.startswith('-Ns -o wega')] + \
        [' '.join(args) for args in check.rnd_seg(50, 1)]


def CORPUS_PREPARE(src, work):
    """the test objects in work, the command line saved for both runs; returns the output file"""
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
    if binary == 'pass_new':        # the program name shows in the messages
        os.rename(os.path.join(work, binary), os.path.join(work, 'sld'))
        binary = 'sld'
    return [binary] + args, None


def CORPUS_OUTPUTS(base):
    return [base]
