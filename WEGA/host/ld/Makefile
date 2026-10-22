# ld, sld: the WEGA loaders built for Linux (cross tools). They write the same files as /bin/ld
# (non-segmented) and /bin/sld (segmented) of WEGA. Sources: ld.c, sld.c, written from the original
# binaries (see ../KNOWLEDGE.md).
#
#   make                 build ./ld and ./sld
#   make check           compare with the original /bin/ld under z8k-emu
#                        (WEGA=<file tree of the original system>)

HOSTCC = cc
CFLAGS = -O2 -Wall
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)

all: ld sld

ld: ld.c
	$(HOSTCC) $(CFLAGS) -o $@ ld.c

sld: sld.c
	$(HOSTCC) $(CFLAGS) -o $@ sld.c

check: ld sld
	WEGA=$(abspath $(WEGA)) EMU=$(EMU) python3 check.py

clean:
	rm -f ld sld

.PHONY: all check clean
