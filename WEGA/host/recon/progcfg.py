"""progcfg.py - the per-program configuration of the recon toolkit.

A program (codgen, popt, sld, ...) keeps a file `progcfg.py` in its tools directory with plain constants:

  NAME        program name, e.g. 'codgen'
  ORIG        path of the original below the WEGA root, e.g. 'lib/codgen'
  OWN_LIM     end of the program part of the original (the C library follows), as an address
  LIBC_FROM   same, used by patterns.py (usually == OWN_LIM)
  OWN_OBJ     regular expression of the object names that belong to the program (for the link map of our build)
  ORIG_BIN    original binary below the WEGA root if it differs from ORIG (sld: 'bin/sld')
  ORIG_LIM    upper address limit for disassembling the original (default OWN_LIM; 0x10000 for stripped files)

The directory is taken from the environment variable RECON_PROG (set by the launchers in the program's
tools directory); if it is not set, the current directory is used.
"""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.join(HERE, '..', 'tools')


def prog_dir():
    return os.path.abspath(os.environ.get('RECON_PROG') or os.getcwd())


def load():
    d = prog_dir()
    spec = importlib.util.spec_from_file_location('progcfg_local', os.path.join(d, 'progcfg.py'))
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    m.DIR = d
    if not hasattr(m, 'ORIG_LIM'):
        m.ORIG_LIM = m.OWN_LIM
    if not hasattr(m, 'ORIG_BIN'):
        m.ORIG_BIN = m.ORIG
    m.ROOT = os.environ.get('WEGA_ROOT', '/mnt/share/cvs/P8000/tmp/wega31_root')
    if not hasattr(m, 'LIBC_FROM'):
        m.LIBC_FROM = m.OWN_LIM
    return m
