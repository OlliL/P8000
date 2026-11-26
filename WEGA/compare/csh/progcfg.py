"""Configuration of the recon toolkit (host/recon/progcfg.py) for csh (nonsegmented); written by host/recon/setup.py."""
NAME = 'csh'
ORIG = 'bin/csh'
OWN_LIM = 0x8db2    # the own code: crt0 and the program; the libc (/lib/libc.a) starts with _callex here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'
