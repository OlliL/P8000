"""Configuration of the recon toolkit (host/recon/progcfg.py) for sum (nonsegmented)."""
NAME = 'sum'
ORIG = 'bin/sum'
OWN_LIM = 0x022e    # the own code: crt0 and main; the first libc function is fopen at 0x022e
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0'    # dated 07/82, D1 tight (OLDCC_DATES.md); under WEGA_ROOT=$OLDROOT
