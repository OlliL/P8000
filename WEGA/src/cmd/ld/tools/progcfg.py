"""Configuration of the recon toolkit (host/recon/progcfg.py) for ld (nonsegmented, stripped)."""
NAME = 'ld'
ORIG = 'bin/ld'
OWN_LIM = 0x2c4c    # the own code of /bin/ld ends here; shipped libc members follow (libcver.py)
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|copyname|printname|symbols|ld)'
