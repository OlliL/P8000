"""Corpus of the segmented C compiler chain (host/tools/corpus.sh scchain): as compare/cchain with scc and the segmented passes (scparse, scodgen,
spopt, cas, sld); the inputs are the ones of cchain (compare/cchain/corpus.tar)."""
import importlib.util
import os

_here = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location('progcfg_cchain', os.path.join(_here, '..', 'cchain', 'progcfg.py'))
_b = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_b)

NAME = 'scchain'
ORIG = 'bin/scc'
PROG_NAME = 'scc'
CORPUS_OF = 'cchain'
CANDIDATES = {'host': 'host/cc/scc'}
CORPUS_VARIANTS = _b.CORPUS_VARIANTS
PASSES = {'cpp': ('host/cpp/cpp', 'lib/cpp'), 'cparse': ('host/cparse/scparse', 'lib/scparse'), 'codgen': ('host/codgen/scodgen', 'lib/scodgen'),
          'popt': ('host/popt/spopt', 'lib/spopt'), 'cas': ('host/cas/cas', 'bin/cas'), 'ld': ('host/ld/sld', 'bin/sld')}
CRT0, LIBC, CONFNAME = 'lib/scrt0.o', 'lib/slibc.a', 'scc.conf'
CORPUS_DEPENDS = {'host': [v[0] for v in PASSES.values()]}
CORPUS_ROOT_FILES = ['usr/include', 'bin/mv', CRT0, LIBC] + [v[1] for v in PASSES.values()]
CORPUS_SOURCES = _b.CORPUS_SOURCES
CORPUS_PREPARE = _b.CORPUS_PREPARE
CORPUS_SCRIPT = _b.CORPUS_SCRIPT
CORPUS_NORMALIZE = _b.CORPUS_NORMALIZE


def CORPUS_HOST_ENV(root):
    b = _b
    conf = ''.join('%s = %s\n' % (k, os.path.join(b.REPO, v[0])) for k, v in PASSES.items())
    conf += 'crt0 = %s\nlibc = %s\ntmpdir = .\n' % (os.path.join(root, CRT0), os.path.join(root, LIBC))
    return {'CPP_INCLUDE': os.path.join(root, 'usr', 'include'), 'CHAIN_CONF': conf, 'CHAIN_CONFNAME': CONFNAME}
