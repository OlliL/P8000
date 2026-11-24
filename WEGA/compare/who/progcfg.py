"""Configuration of the recon toolkit (host/recon/progcfg.py) for who (nonsegmented); written by host/recon/setup.py."""
NAME = 'who'
ORIG = 'bin/who'
OWN_LIM = 0x01e0    # the own code: crt0 and the program; the first libc function (getgrgid) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
