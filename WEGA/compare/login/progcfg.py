"""Configuration of the recon toolkit (host/recon/progcfg.py) for login (nonsegmented); written by host/recon/setup.py."""
NAME = 'login'
ORIG = 'bin/login'
OWN_LIM = 0x0df2    # the own code: crt0 and the program; the first libc function (getpwnam) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
