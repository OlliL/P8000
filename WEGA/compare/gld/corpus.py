"""Corpus of gld (host/recon/corpusrun.py): the fixed and random command lines of fixtures.py on the segmented test objects of
sld and the libraries and damaged objects of gld (made once by the original tools); the original against the host build
(gld has no WEGA source yet)."""
import glob
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_s = importlib.util.spec_from_file_location('ld_corpus', os.path.join(HERE, '..', 'ld', 'corpus.py'))
_ld = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_ld)

NAME = 'gld'
ORIG = 'bin/gld'
PROG = 'gld'
CANDIDATES = {'host': 'host/ld/gld'}
ROOT_FILES = ['lib/scrt0.o', 'lib/slibc.a', 'lib/slibib.a', 'usr/lib/slibib.a', 'z/bin/lib/slibib.a']
PREP_FILES = ['bin/scc', 'bin/cas', 'bin/ar', 'usr/bin/ranlib', 'lib/cpp', 'lib/scparse', 'lib/scodgen', 'lib/spopt',
              'usr/include', 'usr/sys']


def prepare(src, work):
    return _ld.made_in(work, [(_ld.fixtures('sld'), 'setup_seg'), (_ld.fixtures('gld'), 'setup_gld')], 'sbase')


def sources():
    return sorted(glob.glob(os.path.join(HERE, '..', 'ld', 'test', '*'))) + \
        [os.path.join(HERE, '..', 'sld', 'fixtures.py'), os.path.join(HERE, 'fixtures.py')]


def generate(g):
    fx = _ld.fixtures('gld')
    lines = list(fx.GFIXED) + [' '.join(a) for a in fx.rnd_gld(50, 1)]
    for i, l in enumerate(lines):
        text = 'step: gld %s\n' % l
        if i == 1:
            text += 'expect: exit 0\nexpect: file t contains _main\n'
        yield g.case(None, prepare, text, name='%s%03d' % ('f' if i < len(fx.GFIXED) else 'r', i),
                     sources=sources(), prep_files=PREP_FILES)
