# ld: the WEGA non-segmented loader built for Linux (a cross tool). It writes the same
# files as /bin/ld of WEGA. Source: ld.c, written from the original binary (see ../KNOWLEDGE.md).
#
#   make                 build ./ld
#   make check           compare with the original /bin/ld under z8k-emu
#                        (WEGA=<file tree of the original system>)

HOSTCC = cc
CFLAGS = -O2 -Wall
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)

all: ld

ld: ld.c
	$(HOSTCC) $(CFLAGS) -o $@ ld.c

check: ld
	WEGA=$(abspath $(WEGA)) EMU=$(EMU) python3 check.py

clean:
	rm -f ld

.PHONY: all check clean
