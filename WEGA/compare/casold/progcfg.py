"""Configuration of the recon toolkit (host/recon/progcfg.py) for the old cas (src/cmd/cas/old, /lib/old/cas of the OLDROOT);
the corpus: compare/casold/corpus.py."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_spec = importlib.util.spec_from_file_location('progcfg_cas', os.path.join(HERE, '..', 'cas', 'progcfg.py'))
_base = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(_base)

NAME = 'casold'
ORIG = 'lib/old/cas'
OWN_LIM = _base.OWN_LIM
ORIG_LIM = _base.ORIG_LIM
SEG = True
CC = _base.CC
ROOT_ENV = 'OLDROOT'
