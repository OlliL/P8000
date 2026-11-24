"""Configuration of the recon toolkit (host/recon/progcfg.py) for who (nonsegmented); written by host/recon/setup.py."""
NAME = 'who'
ORIG = 'bin/who'
OWN_LIM = 0x01e0    # the own code: crt0 and the program; the first libc function (getgrgid) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccvm'    # the compiler of 11/18/83 (D1 register slots, OLD_D1S); old compilers under WEGA_ROOT=$OLDROOT
