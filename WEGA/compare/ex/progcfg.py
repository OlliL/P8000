"""Configuration of the recon toolkit (host/recon/progcfg.py) for ex (nonsegmented); written by host/recon/setup.py."""
NAME = 'ex'
ORIG = 'bin/ex'
OWN_LIM = 0xe96e    # the own code: crt0 and the program; the library part (libtermlib-v1.a: termcap, then libc-v1.a) starts here
ORIG_LIM = 0x10000  # the original is stripped
CC = '/bin/ccv0a'   # v0a (findings.json); the dating (D1 tight) points to v0; old compilers under WEGA_ROOT=$OLDROOT
