"""Configuration of the recon toolkit (host/recon/progcfg.py) for dog (nonsegmented); written by host/recon/setup.py."""
NAME = 'dog'
ORIG = 'bin/dog'
OWN_LIM = 0x03a8    # the own code: crt0 and the program; the first libc function (fopen) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
