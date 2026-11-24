"""Configuration of the recon toolkit (host/recon/progcfg.py) for echo2 (nonsegmented); written by host/recon/setup.py."""
NAME = 'echo2'
ORIG = 'bin/echo2'
OWN_LIM = 0x010a    # the own code: crt0 and the program; the first libc function (fputs) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0'    # the candidate of the dating (D1 tight); old compilers under WEGA_ROOT=$OLDROOT
