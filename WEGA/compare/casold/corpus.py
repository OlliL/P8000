"""Corpus of the old cas (host/recon/corpusrun.py): the host build host/cas/casold (-DOLD_CAS: the early decision of jpr and
callr, host/OLDCC.md D17) against the old cas of src/cmd/cas (make oldcas, installed as /lib/old/cas of the OLDROOT). There is
no original binary of the old cas: the reference is the build from src/cmd/cas/old, run by the emulator in the OLDROOT.
The cases are those of cas (compare/cas/corpus.py; the prepared inputs are shared), old_max.s too (the special rules of the
old cas: undecided entries at a {, the limit of 255 entries, a jpr at its label)."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location('corpus_cas', os.path.join(HERE, '..', 'cas', 'corpus.py'))
_base = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_base)

NAME = 'casold'
ORIG = 'lib/old/cas'
PROG = 'cas'
ROOT_ENV = 'OLDROOT'
CANDIDATES = {'host': 'host/cas/casold'}
after_step = _base.after_step
normalize = _base.normalize


def generate(g):
    return _base.generate(g, ())
