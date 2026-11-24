"""Configuration of the recon toolkit (host/recon/progcfg.py) for lex (nonsegmented); written by host/recon/setup.py."""
NAME = 'lex'
ORIG = 'usr/bin/lex'
OWN_LIM = 0x4bd4    # the own code: crt0 and the program (and liby); the first libc function (fopen) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1l'   # the compiler of lex (host/OLDCC.md, v1l); old compilers under WEGA_ROOT=$OLDROOT
