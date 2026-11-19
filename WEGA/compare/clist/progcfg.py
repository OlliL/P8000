"""Configuration of the recon toolkit (host/recon/progcfg.py) for clist (nonsegmented); written by host/recon/setup.py."""
NAME = 'clist'
ORIG = 'lib/clist'
OWN_LIM = 0x384     # the own code: crt0 and the program; libc-v2 (fputs first) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0p'    # with -I src/head/oldv7; ccv0 gives the same bytes; libc-v2
