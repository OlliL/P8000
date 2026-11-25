"""Configuration of the recon toolkit (host/recon/progcfg.py) for local (nonsegmented); written by host/recon/setup.py."""
NAME = 'local'
ORIG = 'bin/local'
OWN_LIM = 0x03aa    # the own code: crt0 and the program; the first libc function (scanf) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccvz'    # the cparse of 02/84 (OLD_D12B); libc-v14
