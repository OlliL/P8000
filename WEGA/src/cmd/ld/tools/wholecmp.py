#!/usr/bin/env python3
"""wholecmp.py - launcher: runs host/recon/strictcmp.py with the configuration progcfg.py of this directory. The masked comparison (-m): a first look only, see strictcmp."""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECON = os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon')
os.environ['RECON_PROG'] = HERE
sys.path.insert(0, RECON)
sys.argv[0] = os.path.join(RECON, 'strictcmp.py')
sys.argv.insert(1, '-m')
runpy.run_path(sys.argv[0], run_name='__main__')
