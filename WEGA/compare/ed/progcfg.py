"""Configuration of the recon toolkit (host/recon/progcfg.py) for ed (nonsegmented); written by host/recon/setup.py."""
NAME = 'ed'
ORIG = 'bin/ed'
OWN_LIM = 0x2e78    # the own code: crt0 and the program; the first libc function (__callex, then exit, execlp, execvp) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv1'    # the candidate of the dating (D1 slots); old compilers under WEGA_ROOT=$OLDROOT
