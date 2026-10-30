"""progcfg.py - the per-program configuration of the recon toolkit.

A program (codgen, popt, sld, ...) keeps a file `progcfg.py` in its tools directory with plain constants:

  NAME        program name, e.g. 'codgen'
  ORIG        path of the original below the WEGA root, e.g. 'lib/codgen'
  OWN_LIM     end of the program part of the original (the C library follows), as an address
  LIBC_FROM   same, used by patterns.py (usually == OWN_LIM)
  OWN_OBJ     regular expression of the object names that belong to the program (for the link map of our build)
  ORIG_BIN    original binary below the WEGA root if it differs from ORIG (sld: 'bin/sld')
  ORIG_LIM    upper address limit for disassembling the original (default OWN_LIM; 0x10000 for stripped files)

Used by strictcmp, fncheck, inventory, own_table, d12model, rename (defaults in brackets):
  SEG         segmented code (False)
  CC          compiler in the emulator ('/bin/cc'; '/bin/scc' for segmented programs)
  SRCDIR      directory of our sources (the parent of the tools directory)
  LINKED      our linked program with symbols (SRCDIR/NAME, made by the Makefile)
  FINDINGS    findings.json of the program (tools directory)
  MODULES     C modules in link order, without .c (for d12model)
  model_link(work, objs)  argv of the link of the d12model objects (objs: our modules' objects in link order)
  MODEL_OUT   output of d12model.py (TMP/d12model_NAME/NAMEm)
  STRICT_INPUT  default input of strictcmp.py (LINKED; MODEL_OUT when our data layout differs only by D12)
  TABLES, DOCS  extra text files for rename.py (beside the sources, findings.json and the .md files of the tools)

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
    m.ROOT = os.environ.get('WEGA_ROOT') or os.environ.get('WEGA') or '/mnt/share/cvs/P8000/tmp/wega31_root'
    if not hasattr(m, 'LIBC_FROM'):
        m.LIBC_FROM = m.OWN_LIM
    defaults = {'SEG': False, 'CC': '/bin/cc', 'SRCDIR': os.path.normpath(os.path.join(d, '..')),
                'MODULES': [], 'TABLES': [], 'DOCS': []}
    for k, v in defaults.items():
        if not hasattr(m, k):
            setattr(m, k, v)
    if not hasattr(m, 'LINKED'):
        m.LINKED = os.path.join(m.SRCDIR, m.NAME)
    if not hasattr(m, 'MODEL_OUT'):
        m.MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_' + m.NAME, m.NAME + 'm')
    if not hasattr(m, 'STRICT_INPUT'):
        m.STRICT_INPUT = m.LINKED
    if not hasattr(m, 'FINDINGS'):
        m.FINDINGS = os.path.join(d, 'findings.json')
    m.EMU = os.environ.get('EMU', os.path.normpath(os.path.join(HERE, '..', 'z8k-emu', 'z8k-emu')))
    m.REPO = os.path.normpath(os.path.join(HERE, '..', '..'))    # the WEGA directory of the repository
    return m
