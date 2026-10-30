#!/usr/bin/env python3
"""oldlibc_match.py - launcher: runs host/recon/oldlibc_match.py."""
import os
import runpy
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RECON = os.path.join(HERE, '..', '..', '..', '..', 'host', 'recon')
sys.path.insert(0, RECON)
sys.argv[0] = os.path.join(RECON, 'oldlibc_match.py')
runpy.run_path(sys.argv[0], run_name='__main__')
