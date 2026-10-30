#!/usr/bin/env python3
"""d12model.py - launcher: runs host/recon/d12model.py with the configuration progcfg.py of this directory."""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECON = os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon')
os.environ['RECON_PROG'] = HERE
sys.path.insert(0, RECON)
sys.argv[0] = os.path.join(RECON, 'd12model.py')
runpy.run_path(sys.argv[0], run_name='__main__')
