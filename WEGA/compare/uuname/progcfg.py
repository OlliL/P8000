"""Configuration of the recon toolkit (host/recon/progcfg.py) for uuname (nonsegmented); written by host/recon/setup.py."""
NAME = 'uuname'
ORIG = 'bin/uuname'
OWN_LIM = 0x0158    # the own code: crt0 and the program; the first libc function (fgets) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'      # the candidate of the dating (D1 loose); old compilers under WEGA_ROOT=$OLDROOT
