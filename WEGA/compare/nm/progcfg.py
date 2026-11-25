"""Configuration of the recon toolkit (host/recon/progcfg.py) for nm (nonsegmented); written by host/recon/setup.py."""
NAME = 'nm'
ORIG = 'bin/nm'
OWN_LIM = 0x0c08    # the own code: crt0 and the program; the first libc function (getopt) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccvm'    # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
