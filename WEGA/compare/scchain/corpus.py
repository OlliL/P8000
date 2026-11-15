"""Corpus of the segmented C compiler chain (host/recon/corpusrun.py): as compare/cchain with scc and the segmented passes."""
import importlib.util
import os

_s = importlib.util.spec_from_file_location('cchain_corpus', os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'cchain', 'corpus.py'))
_c = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_c)

NAME = 'scchain'
ORIG = 'bin/scc'
PROG = 'scc'
CONFNAME = 'scc.conf'
PASSES = {'cpp': ('host/cpp/cpp', 'lib/cpp'), 'cparse': ('host/cparse/scparse', 'lib/scparse'), 'codgen': ('host/codgen/scodgen', 'lib/scodgen'),
          'popt': ('host/popt/spopt', 'lib/spopt'), 'cas': ('host/cas/cas', 'bin/cas'), 'ld': ('host/ld/sld', 'bin/sld')}
CRT0, LIBC = 'lib/scrt0.o', 'lib/slibc.a'
CANDIDATES = {'host': 'host/cc/scc'}
DEPENDS, ROOT_FILES = _c.derived(PASSES, CRT0, LIBC)
HOST_ENV = _c.HOST_ENV
host_setup = _c.make_host_setup(PASSES, CRT0, LIBC, CONFNAME)
normalize = _c.normalize


def generate(g):
    return _c.generate_for(g, PROG)
