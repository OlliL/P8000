"""Configuration of the recon toolkit (host/recon/progcfg.py) for tr (nonsegmented); written by host/recon/setup.py."""
NAME = 'tr'
ORIG = 'bin/tr'
OWN_LIM = 0x04b0    # the own code: crt0 and the program; the first libc function (printf) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
