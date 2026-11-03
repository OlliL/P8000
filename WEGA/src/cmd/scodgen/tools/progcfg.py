"""Configuration of the recon toolkit (host/recon/progcfg.py) for scodgen: the sources of codgen with -DSEG."""
import importlib.util
import os

HERE = os.path.dirname(os.path.abspath(__file__))
_s = importlib.util.spec_from_file_location('codgen_progcfg', os.path.join(HERE, '..', '..', 'codgen', 'tools', 'progcfg.py'))
_p = importlib.util.module_from_spec(_s)
_s.loader.exec_module(_p)
globals().update({k: v for k, v in vars(_p).items() if k.isupper()})

NAME = 'scodgen'
ORIG = 'lib/scodgen'
OWN_LIM = 0xbf00    # to be measured
SRCDIR = os.path.normpath(os.path.join(HERE, '..', '..', 'codgen'))
CCFLAGS = ['-DSEG']
MODULES = [m.replace('table3_codgen', 'table3_scodgen') for m in _p.MODULES]
model_link = _p.model_link
MODEL_OUT = os.path.join(os.environ.get('RECON_TMP', '/mnt/share/cvs/P8000/tmp/tmp'), 'd12model_scodgen', os.path.basename(_p.MODEL_OUT))
STRICT_INPUT = MODEL_OUT
