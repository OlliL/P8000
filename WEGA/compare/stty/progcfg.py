"""Configuration of the recon toolkit (host/recon/progcfg.py) for stty (nonsegmented); written by host/recon/setup.py."""
NAME = 'stty'
ORIG = 'bin/stty'
OWN_LIM = 0x0eae    # the own code: crt0 and the program; the first libc function (scanf) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
