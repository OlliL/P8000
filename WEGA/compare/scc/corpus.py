"""Corpus of the driver scc (host/recon/corpusrun.py): the command lines of compare/cc/cmdlines.txt (see compare/cc/corpus.py)."""
import importlib.util
import os

_s = importlib.util.spec_from_file_location('cc_corpus', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'cc', 'corpus.py'))
_c = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_c)

NAME = 'scc'
ORIG = 'bin/scc'
PROG = 'scc'
CANDIDATES = {'wega': 'src/cmd/cc/build/seg/scc', 'host': 'host/cc/scc'}
normalize = _c.normalize


def generate(g):
    yield g.case(None, None, _c.lines_case(PROG), name='cmdlines')
