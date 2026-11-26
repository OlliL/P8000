"""Configuration of the recon toolkit (host/recon/progcfg.py) for adb (nonsegmented)."""
NAME = 'adb'
ORIG = 'bin/adb'
OWN_LIM = 0x505c    # the own code: crt0, the program and fpcvt.s; libu8000 (disasm) follows, then kva.o, kpa.o, /lib/libc.a
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/cc'
