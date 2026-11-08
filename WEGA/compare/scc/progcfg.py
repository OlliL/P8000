"""Corpus of the segmented C compiler driver scc (host/tools/corpus.sh scc): the command lines of cases.sh (script mode), the original against the WEGA build
and the host build."""
import re

NAME = 'scc'
ORIG = 'bin/scc'
CANDIDATES = {'wega': 'src/cmd/cc/build/seg/scc', 'host': 'host/cc/scc'}


def CORPUS_SOURCES():
    return ['scc']


def CORPUS_PREPARE(src, work):
    return src


def CORPUS_SCRIPT(base):
    return '../cc/cases.sh'


def CORPUS_NORMALIZE(name, data):
    """the process id in the names of the temporary files (cc.<pid>.<x>)"""
    return re.sub(rb'cc\.-?[0-9]+', b'cc.PID', data) if name == 'messages' else data
