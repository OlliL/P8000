# popt: C optimizer pass of the WEGA C compiler (z20.c tables, z21.c main/flow,
# z22.c register tracking, z23.c liveness and register renaming).
CFLAGS ?= -O2 -g -Wall -Wno-unused -Wno-format-truncation
WEGA ?= ../../../wega/arc/cpio
CORPUS ?= /tmp/popt-corpus

all: popt

popt: z20.c z21.c z22.c z23.c z2.h
	$(CC) $(CFLAGS) -o $@ z20.c z21.c z22.c z23.c

# Compare with the original popt on a corpus (see corpus.sh); needs WEGA.
check: all
	WEGA=$(abspath $(WEGA)) ./check.sh $(CORPUS)

clean:
	rm -f popt

.PHONY: all check clean
