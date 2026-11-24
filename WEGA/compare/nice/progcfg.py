"""Configuration of the recon toolkit (host/recon/progcfg.py) for nice (nonsegmented); written by host/recon/setup.py."""
NAME = 'nice'
ORIG = 'bin/nice'
OWN_LIM = 0x00d8    # the own code: crt0 and the program; the library part starts with fputs (0_2)
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
