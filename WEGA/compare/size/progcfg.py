"""Corpus of size (host/tools/corpus.sh size): the cases of compare/binutils (script mode), the original against the WEGA build
and the host build."""
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', 'binutils'))
import corpus as bc  # noqa: E402

NAME = 'size'
ORIG = 'bin/size'
CANDIDATES = {'wega': 'src/cmd/size', 'host': 'host/binutils/size'}
_TREE = os.path.join(os.environ.get('STAGE_TMP', '/tmp/wega_stage'), 'binutils_tree_size')


def CORPUS_SOURCES():
    bc.build(_TREE)
    return bc.cases(NAME, _TREE)


def CORPUS_PREPARE(src, work):
    return bc.prepare(src, work, _TREE)


def CORPUS_SCRIPT(base):
    return '../binutils/run.sh'
