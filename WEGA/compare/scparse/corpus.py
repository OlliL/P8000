"""Corpus of scparse (host/recon/corpusrun.py): the cases of cparse (compare/cparse/corpus.py) with the segmented pass."""
import importlib.util
import os

_s = importlib.util.spec_from_file_location('cparse_corpus', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'cparse', 'corpus.py'))
_c = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_c)

NAME = 'scparse'
ORIG = 'lib/scparse'
PROG = 'scparse'
CANDIDATES = {'wega': 'src/cmd/cc/cparse/build/seg/scparse', 'host': 'host/cparse/scparse'}
ROOT_FILES = _c.ROOT_FILES
PREP_FILES = _c.PREP_FILES
normalize = _c.normalize


def generate(g):
    for src in _c.sources(g):
        yield g.case(src, _c.corpusc.prepare_i, 'step: %s -E t\nstdin: t.i\n' % PROG + _c.EXPECT.get(g.name(src), ''),
                     prep_files=PREP_FILES)
