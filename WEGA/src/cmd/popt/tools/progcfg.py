"""Configuration of the recon toolkit (host/recon/progcfg.py) for popt."""
NAME = 'popt'
ORIG = 'lib/popt'
OWN_LIM = 0x3838   # the program part of /lib/popt ends here (the old-cc C library follows, NOTES.md)
ORIG_LIM = 0x10000  # the original is stripped
OWN_OBJ = r'^(crt0|z20|z21|z22|z23|crt0-popt)'
