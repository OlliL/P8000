"""Configuration of the recon toolkit (host/recon/progcfg.py) for test (nonsegmented); written by host/recon/setup.py."""
NAME = 'test'
ORIG = 'bin/test'
OWN_LIM = 0x0726    # the own code: crt0 and the program; the first libc function (__callex) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # measured: ccv1 IDENTICAL (cc, ccv0, ccv1r, ccv1q not); old compilers under WEGA_ROOT=$OLDROOT
