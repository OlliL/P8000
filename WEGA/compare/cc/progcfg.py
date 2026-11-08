"""Corpus of the C compiler driver cc (host/tools/corpus.sh cc): the command lines of cases.sh (script mode), the original against the WEGA build
and the host build."""
import re

NAME = 'cc'
ORIG = 'bin/cc'
CANDIDATES = {'wega': 'src/cmd/cc/build/nonseg/cc', 'host': 'host/cc/cc'}


def CORPUS_SOURCES():
    return ['cc']


def CORPUS_PREPARE(src, work):
    return src


def CORPUS_SCRIPT(base):
    return 'cases.sh'


def CORPUS_NORMALIZE(name, data):
    """the process id in the names of the temporary files (cc.<pid>.<x>)"""
    return re.sub(rb'cc\.-?[0-9]+', b'cc.PID', data) if name == 'messages' else data
