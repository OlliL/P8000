"""Configuration of the recon toolkit (host/recon/progcfg.py) for mail (nonsegmented); written by host/recon/setup.py."""
NAME = 'mail'
ORIG = 'bin/mail'
OWN_LIM = 0x130c    # the own code: crt0 and the program; the first libc function (getpwnam) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccvm'    # the compiler of 11/18/83 (D1 register slots, OLD_D1S); old compilers under WEGA_ROOT=$OLDROOT
