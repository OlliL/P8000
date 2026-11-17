"""Configuration of the recon toolkit (host/recon/progcfg.py) for cpio (nonsegmented); written by host/recon/setup.py."""
NAME = 'cpio'
ORIG = 'usr/bin/cpio'
OWN_LIM = 0x16f8    # the own code: crt0 and the program; the first libc function (getgrgid) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
