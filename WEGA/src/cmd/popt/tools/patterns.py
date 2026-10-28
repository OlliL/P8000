"""patterns - library shim: the module is host/recon/patterns.py, configured by progcfg.py of this directory."""
import importlib.util
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECON = os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon')
os.environ.setdefault('RECON_PROG', HERE)
sys.path.insert(0, RECON)
spec = importlib.util.spec_from_file_location('patterns', os.path.join(RECON, 'patterns.py'))
mod = importlib.util.module_from_spec(spec)
sys.modules[__name__] = mod
spec.loader.exec_module(mod)
