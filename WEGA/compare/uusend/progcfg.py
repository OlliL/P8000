"""Configuration of the recon toolkit (host/recon/progcfg.py) for uusend (nonsegmented); written by host/recon/setup.py."""
NAME = 'uusend'
ORIG = 'bin/uusend'
OWN_LIM = 0x05e6    # the own code: crt0, main, index, getfname, makedir; the shipped libc.a follows (getpwnam first)
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
