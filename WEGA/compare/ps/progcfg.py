"""Configuration of the recon toolkit (host/recon/progcfg.py) for ps (segmented, ps.c with kva.c and kpa.c)."""
NAME = 'ps'
ORIG = 'bin/ps'
OWN_LIM = 0x1af6    # the own code of /bin/ps ends here; the members of the shipped /lib/slibc.a follow (getopt first)
ORIG_LIM = 0x10000  # the original is stripped
SEG = True
CC = '/bin/scc'
MODULES = ['ps', 'kva', 'kpa']   # link order of the own modules
