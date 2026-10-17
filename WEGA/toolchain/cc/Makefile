# cc: C compiler driver (WEGA 3.2), reconstructed from the original /bin/cc.
#
#   make all      build the portable driver (host cc), result ./cc
#   make verify   compare the command lines it builds with those of the
#                 original $(WEGA)/bin/cc run under z8k-emu (check.sh)
#
# The driver only runs the pass programs; with -:n it executes nothing and
# with -:z it prints the command lines, which is what check.sh compares.

WEGA ?= ../../../../wega/arc/cpio
EMU  ?= $(abspath ../z8k-emu/z8k-emu)
CC   ?= cc
CFLAGS ?= -O2 -std=gnu89 -w

all: cc

cc: cc.c
	$(CC) $(CFLAGS) -o $@ cc.c

verify: cc
	WEGA=$(abspath $(WEGA)) E=$(EMU) ./check.sh

clean:
	rm -f cc

.PHONY: all verify clean
