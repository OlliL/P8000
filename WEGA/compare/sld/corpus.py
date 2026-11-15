"""Corpus of sld (host/recon/corpusrun.py): the fixed and random command lines of fixtures.py on the segmented test objects
(made once by the original scc and cas; with the kernel objects of usr/sys when the tree has them); the original against the
shipped build and the host build. The settings of the reconstruction tools stay in progcfg.py."""
import glob
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_s = importlib.util.spec_from_file_location('ld_corpus', os.path.join(HERE, '..', 'ld', 'corpus.py'))
_ld = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_ld)

NAME = 'sld'
ORIG = 'bin/sld'
PROG = 'sld'
CANDIDATES = {'wega': 'src/cmd/sld/sld', 'host': 'host/ld/sld'}
ROOT_FILES = ['lib/scrt0.o', 'lib/slibc.a']
PREP_FILES = ['bin/scc', 'bin/cas', 'lib/cpp', 'lib/scparse', 'lib/scodgen', 'lib/spopt', 'usr/include', 'usr/sys']


def prepare(src, work):
    return _ld.made_in(work, [(_ld.fixtures('sld'), 'setup_seg')], 'sbase')


def sources():
    return sorted(glob.glob(os.path.join(HERE, '..', 'ld', 'test', '*'))) + [os.path.join(HERE, 'fixtures.py')]


def generate(g):
    fx = _ld.fixtures('sld')
    kernel = os.path.isdir(os.path.join(fx.ROOT, 'usr', 'sys'))
    lines = [l for l in fx.SFIXED if kernel or not l.startswith('-Ns -o wega')]
    nfixed = len(lines)
    lines += [' '.join(a) for a in fx.rnd_seg(50, 1)]
    for i, l in enumerate(lines):
        text = 'step: sld %s\n' % l
        if i == 0:
            text += 'expect: exit 0\nexpect: file t contains _main\n'
        yield g.case(None, prepare, text, name='%s%03d' % ('f' if i < nfixed else 'r', i), sources=sources(),
                     prep_files=PREP_FILES)
