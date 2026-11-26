"""Configuration of the recon toolkit (host/recon/progcfg.py) for sh (nonsegmented); written by host/recon/setup.py."""
NAME = 'sh'
ORIG = 'bin/sh'
OWN_LIM = 0x4c3a    # the own code: crt0 and the program; the libc starts with _callex here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
