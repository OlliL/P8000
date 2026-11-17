"""Configuration of the recon toolkit (host/recon/progcfg.py) for pr (nonsegmented); written by host/recon/setup.py."""
NAME = 'pr'
ORIG = 'bin/pr'
OWN_LIM = 0x0f78    # the own code: crt0 and the program; the first libc function (__callex) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
