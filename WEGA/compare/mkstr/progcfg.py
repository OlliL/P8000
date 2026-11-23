"""Configuration of the recon toolkit (host/recon/progcfg.py) for mkstr (nonsegmented); written by host/recon/setup.py."""
NAME = 'mkstr'
ORIG = 'usr/bin/mkstr'
OWN_LIM = 0x06cc    # the own code: crt0 (0x00-0x42) and the 8 functions of mkstr.c; libc from here (measured by the callers)
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0'    # ccv0 and -I../head/oldv7, all exact (findings.json)
