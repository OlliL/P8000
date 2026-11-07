"""Corpus of the host ar (host/tools/corpus.sh --host ar DIR, see host/recon/stagecorpus.py, script mode): the scenarios of
scenarios.sh (every command, its output and exit status, the archives and files afterwards) for several time zones, and the
listing of the libraries of the system."""
import os
import shutil

NAME = 'ar'
ORIG = 'bin/ar'
CORPUS_BINARY = 'ar'

# the default time zone of WEGA (EST5EDT) and some set by TZ, which tv prints
CORPUS_VARIANTS = [{}, {'TZ': 'MET-1MEST'}, {'TZ': 'PST8PDT'}, {'TZ': 'JST-9'}, {'TZ': 'GMT0'}]

LIBS = ['libc.a', 'slibc.a']


def CORPUS_SOURCES():
    return ['scenario'] + LIBS


def CORPUS_PREPARE(src, work):
    if src in LIBS:
        import corpusc
        shutil.copy(os.path.join(corpusc.root(), 'lib', src), work)
    return src


def CORPUS_SCRIPT(base):
    return 'lib.sh' if base in LIBS else 'scenarios.sh'
