"""Configuration of the recon toolkit (host/recon/progcfg.py) for dd (nonsegmented); written by host/recon/setup.py."""
NAME = 'dd'
ORIG = 'bin/dd'
OWN_LIM = 0x0834    # the own code: crt0 and the program; the first libc function (printf) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1r'   # measured: ccv1r (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
