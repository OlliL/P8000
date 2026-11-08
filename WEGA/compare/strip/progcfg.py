"""Corpus of strip (host/tools/corpus.sh strip): the cases of compare/binutils (script mode), the original against the WEGA build
and the host build."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'binutils'))
import corpus as bc  # noqa: E402

NAME = 'strip'
ORIG = 'usr/bin/strip'
CANDIDATES = {'wega': 'src/cmd/strip', 'host': 'host/binutils/strip'}
_TREE = os.path.join(os.environ.get('STAGE_TMP', '/tmp/wega_stage'), 'binutils_tree_strip')


def CORPUS_SOURCES():
    bc.build(_TREE)
    return bc.cases(NAME, _TREE)


def CORPUS_PREPARE(src, work):
    return bc.prepare(src, work, _TREE)


def CORPUS_SCRIPT(base):
    return '../binutils/run.sh'
