# popt: C optimizer pass of the WEGA C compiler (z20.c tables, z21.c main/flow,
# z22.c register tracking, z23.c liveness and register renaming).
SRC ?= ../../src/cmd/popt
CFLAGS ?= -O2 -g -Wall -Wno-unused -Wno-format-truncation
WEGA ?= ../../../wega/arc/cpio
CORPUS ?= /tmp/popt-corpus

all: popt

popt: $(SRC)/z20.c $(SRC)/z21.c $(SRC)/z22.c $(SRC)/z23.c $(SRC)/z2.h
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $(SRC)/z20.c $(SRC)/z21.c $(SRC)/z22.c $(SRC)/z23.c

# Compare with the original popt on a corpus (see corpus.sh); needs WEGA.
check: all
	WEGA=$(abspath $(WEGA)) ./check.sh $(CORPUS)

clean:
	rm -f popt

.PHONY: all check clean
