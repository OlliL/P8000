"""Corpus of ld (host/recon/corpusrun.py): the fixed and random command lines of fixtures.py on its test objects and
archives (made once by the original cc, ar, ranlib: one prepared directory for all cases); the original against the shipped
build and the host build. sld and gld (compare/sld, compare/gld) use the same pattern. The settings of the reconstruction
tools stay in progcfg.py."""
import glob
import importlib.util
import os
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'host', 'recon'))
import corpusc  # noqa: E402

NAME = 'ld'
ORIG = 'bin/ld'
PROG = 'ld'
CANDIDATES = {'wega': 'src/cmd/ld/ld', 'host': 'host/ld/ld'}
ROOT_FILES = ['lib/crt0.o', 'lib/libc.a']
PREP_FILES = ['bin/cc', 'bin/ar', 'usr/bin/ranlib', 'bin/ld', 'lib/libc.a', 'lib/crt0.o', 'lib/cpp', 'lib/cparse', 'lib/codgen',
              'lib/popt', 'bin/cas', 'usr/include']


def fixtures(name):
    """the fixtures.py of compare/NAME, set up for the original tree"""
    spec = importlib.util.spec_from_file_location('fixtures_' + name, os.path.join(HERE, '..', name, 'fixtures.py'))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    m.ROOT = corpusc.root()
    m.EMU = corpusc.emu()
    return m


def made_in(work, setups, sub):
    """runs the setups (fixtures, function name) in a scratch directory and moves its directory sub into work"""
    w = tempfile.mkdtemp(prefix='corpus_fixtures_')
    try:
        for fx, fn in setups:
            fx.W = w
            getattr(fx, fn)()
        for f in os.listdir(os.path.join(w, sub)):
            shutil.move(os.path.join(w, sub, f), work)
    finally:
        shutil.rmtree(w, ignore_errors=True)
    return 'ok'


def prepare(src, work):
    return made_in(work, [(fixtures('ld'), 'setup')], 'base')


def sources():
    return sorted(glob.glob(os.path.join(HERE, 'test', '*'))) + [os.path.join(HERE, 'fixtures.py')]


def generate(g):
    fx = fixtures('ld')
    lines = list(fx.FIXED) + [' '.join(a) for a in fx.rnd(100, 1)]
    for i, l in enumerate(lines):
        text = 'step: ld %s\n' % l
        if i == 0:
            text += 'expect: exit 0\nexpect: file t contains _main\n'
        yield g.case(None, prepare, text, name='%s%03d' % ('f' if i < len(fx.FIXED) else 'r', i), sources=sources(),
                     prep_files=PREP_FILES)
