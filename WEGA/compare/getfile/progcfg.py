"""Configuration of the recon toolkit (host/recon/progcfg.py) for getfile (nonsegmented); written by host/recon/setup.py."""
NAME = 'getfile'
ORIG = 'bin/getfile'
OWN_LIM = 0x04ee    # the own code: crt0 and the program; the first libc function (__callex) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccvm'    # the compiler of 11/18/83 (D1 register slots, OLD_D1S); old compilers under WEGA_ROOT=$OLDROOT
