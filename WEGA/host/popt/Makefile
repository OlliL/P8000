# popt: the WEGA peephole optimizer built for Linux (a cross tool, see
# ../cc/Makefile). Source: ../../src/cmd/popt. The byte-level check against
# the original /lib/popt is check.sh (single runs) and corpus.sh (corpus).
#
#   make                 build ./popt
#   make check           compare with the original /lib/popt under z8k-emu

HOSTCC ?= cc
CFLAGS ?= -O2 -g -Wall -Wno-unused -Wno-format-truncation
SRC = ../../src/cmd/popt
WEGA ?= ../../../../wega/arc/cpio
CORPUS ?= $(abspath ../../../../wega/work/popt-corpus)
HOSTINC = -include stdio.h -include ctype.h -include string.h -include stdlib.h

all: popt

popt: $(SRC)/z20.c $(SRC)/z21.c $(SRC)/z22.c $(SRC)/z23.c $(SRC)/z2.h
	$(HOSTCC) $(CFLAGS) $(HOSTINC) -I$(SRC) -o $@ $(SRC)/z20.c $(SRC)/z21.c $(SRC)/z22.c $(SRC)/z23.c

check: all
	WEGA=$(abspath $(WEGA)) ./check.sh $(CORPUS)

# build the corpus (compiler output of all sources in ../../src, slow)
corpus:
	WEGA=$(abspath $(WEGA)) sh corpus.sh $(SRC) $(CORPUS)

clean:
	rm -f popt

.PHONY: all check corpus clean
