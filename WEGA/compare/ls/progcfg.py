"""Configuration of the recon toolkit (host/recon/progcfg.py) for ls (nonsegmented); written by host/recon/setup.py."""
NAME = 'ls'
ORIG = 'bin/ls'
OWN_LIM = 0x12ec    # the own code: crt0, ls and its printf/_doprnt; the first libc function (getgrgid) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
