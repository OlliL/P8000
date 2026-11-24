"""Configuration of the recon toolkit (host/recon/progcfg.py) for od (nonsegmented); written by host/recon/setup.py."""
NAME = 'od'
ORIG = 'bin/od'
OWN_LIM = 0x071c    # the own code: crt0 and the program; the first libc function (freopen) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1r'   # measured: ccv1r (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
