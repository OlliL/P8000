"""Configuration of the recon toolkit (host/recon/progcfg.py) for uux (nonsegmented); written by host/recon/setup.py."""
NAME = 'uux'
ORIG = 'bin/uux'
OWN_LIM = 0x1f88    # the own code: crt0 and the program; the first libc function (getpwnam) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
