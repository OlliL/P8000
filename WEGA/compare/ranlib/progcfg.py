"""Corpus of ranlib (host/tools/corpus.sh ranlib): the cases of compare/binutils (script mode), the original against the WEGA build
and the host build."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'binutils'))
import corpus as bc  # noqa: E402

NAME = 'ranlib'
ORIG = 'usr/bin/ranlib'
CANDIDATES = {'wega': 'src/cmd/ranlib', 'host': 'host/binutils/ranlib'}
_TREE = os.path.join(os.environ.get('STAGE_TMP', '/tmp/wega_stage'), 'binutils_tree_ranlib')


def CORPUS_SOURCES():
    bc.build(_TREE)
    return bc.cases(NAME, _TREE)


def CORPUS_PREPARE(src, work):
    return bc.prepare(src, work, _TREE)


def CORPUS_SCRIPT(base):
    return '../binutils/run.sh'


CORPUS_DEPENDS = {'host': ['host/ar/ar']}	# the programs the candidate runs (part of the cache key)


def CORPUS_HOST_ENV(root):
    """the host ranlib lets the host ar insert the table (RANLIB_AR)"""
    return {'RANLIB_AR': os.path.normpath(os.path.join(HERE, '..', '..', 'host', 'ar', 'ar'))}
