"""Corpus of the old cas (host/cas/casold, -DOLD_CAS: the early decision of jpr and callr, host/OLDCC.md D17) against the old cas of
src/cmd/cas (make oldcas, installed as /lib/old/cas of the OLDROOT). There is no original binary of the old cas: the reference
is the build from src/cmd/cas/old, run by the emulator; the candidate is the host build.

The inputs are the ones of the corpus of cas (compare/cas): the same test sources, among them the fixed sources
compare/cas/test/jpr/old_*.s for the special rules of the old cas (undecided entries at a {, the limit of 255 entries, a jpr at its label)."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location('progcfg_cas', os.path.join(HERE, '..', 'cas', 'progcfg.py'))
_base = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_base)

_base.OLD = True
NAME = 'casold'
ORIG = 'lib/old/cas'
OWN_LIM = _base.OWN_LIM
ORIG_LIM = _base.ORIG_LIM
SEG = True
CC = _base.CC
ROOT_ENV = 'OLDROOT'
CANDIDATES = {'host': 'host/cas/casold'}
CORPUS_BINARY = 'old/cas'

CORPUS_SOURCES = _base.CORPUS_SOURCES
CORPUS_PREPARE = _base.CORPUS_PREPARE


def CORPUS_SCRIPT(base):
    return os.path.join('..', 'cas', 'case.sh')
