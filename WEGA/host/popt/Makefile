# popt: the WEGA peephole optimizer built for Linux (a cross tool, see
# ../cc/Makefile). Decompiled from the original /lib/popt binary; NOTES.md
# lists the reproduced quirks. The byte-level check against the original
# is check.sh (single runs) and corpus.sh (corpus).
#
#   make                 build ./popt and ./spopt
#   make check           compare with the original /lib/popt under z8k-emu
#   make corpus          build the test corpus (slow)

HOSTCC ?= cc
CFLAGS ?= -O2 -g -Wall -Wno-unused -Wno-format-truncation
WEGA ?= ../../../../wega/arc/cpio
CORPUS ?= $(abspath ../../../../wega/work/popt-corpus)
SRC = .

all: popt spopt

popt: z20.c z21.c z22.c z23.c z2.h
	$(HOSTCC) $(CFLAGS) -o $@ z20.c z21.c z22.c z23.c

# spopt (segmented, scc -O): the same sources with -DSPOPT
spopt: z20.c z21.c z22.c z23.c z2.h
	$(HOSTCC) $(CFLAGS) -DSPOPT -o $@ z20.c z21.c z22.c z23.c

check: all
	WEGA=$(abspath $(WEGA)) ./check.sh $(CORPUS)

# build the corpus (compiler output of all sources in ../../src, slow)
corpus:
	WEGA=$(abspath $(WEGA)) sh corpus.sh ../../src $(CORPUS)

clean:
	rm -f popt spopt

.PHONY: all check corpus clean
