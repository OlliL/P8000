"""Configuration of the recon toolkit (host/recon/progcfg.py) for scparse: the sources of cparse with -DSEG."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_s = importlib.util.spec_from_file_location('cparse_progcfg', os.path.join(HERE, '..', 'cparse', 'progcfg.py'))
_p = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_p)
_p.REPO = os.path.normpath(os.path.join(HERE, '..', '..'))    # as progcfg.load does for cparse
globals().update({k: v for k, v in vars(_p).items() if k.isupper()})

NAME = 'scparse'
ORIG = 'lib/scparse'
OWN_LIM = 0xa056    # the own code ends here; the libc part follows
SRCDIR = os.path.normpath(os.path.join(HERE, '..', '..', 'src', 'cmd', 'cparse'))
CCFLAGS = ['-DSEG']
model_link = _p.model_link
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_scparse', os.path.basename(_p.MODEL_OUT))
STRICT_INPUT = MODEL_OUT
CORPUS_BINARY = 'build/seg/scparse'
