#!/usr/bin/env python3
"""libc_spec.py - launcher: runs host/recon/libc_spec.py."""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECON = os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon')
sys.path.insert(0, RECON)
sys.argv[0] = os.path.join(RECON, 'libc_spec.py')
runpy.run_path(sys.argv[0], run_name='__main__')
