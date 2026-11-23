"""Configuration of the recon toolkit (host/recon/progcfg.py) for bc (nonsegmented); written by host/recon/setup.py."""
NAME = 'bc'
ORIG = 'bin/bc'
OWN_LIM = 0x12ee    # the own code: crt0 and the program; the first libc function (fopen) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0'    # the candidate of the dating (D1 tight); old compilers under WEGA_ROOT=$OLDROOT
