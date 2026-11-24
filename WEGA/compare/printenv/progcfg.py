"""Configuration of the recon toolkit (host/recon/progcfg.py) for printenv (nonsegmented); written by host/recon/setup.py."""
NAME = 'printenv'
ORIG = 'bin/printenv'
OWN_LIM = 0x0108    # the own code: crt0 and the program; the first libc function (printf) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0'    # the candidate of the dating (D1 tight); old compilers under WEGA_ROOT=$OLDROOT
