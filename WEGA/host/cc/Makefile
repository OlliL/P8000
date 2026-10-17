# cc: the WEGA C compiler driver built for Linux (a cross tool: it behaves
# like /bin/cc of WEGA; it runs the passes, which are the WEGA programs run
# under z8k-emu for now). Source: cc.c, decompiled from the original binary.
#
#   make                 build ./cc
#   make check           compare with the original /bin/cc under z8k-emu

HOSTCC ?= cc
CFLAGS ?= -O2 -w
WEGA ?= ../../../../wega/arc/cpio
EMU  ?= $(abspath ../z8k-emu/z8k-emu)

all: cc

cc: cc.c
	$(HOSTCC) $(CFLAGS) -std=gnu89 -o $@ cc.c

check: cc
	WEGA=$(abspath $(WEGA)) E=$(EMU) sh check.sh

clean:
	rm -f cc

.PHONY: all check clean
