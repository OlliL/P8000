# popt, spopt: the WEGA peephole optimizer of cc and scc built for Linux (a cross tool: it behaves like /lib/popt and /lib/spopt).
# Sources: z20.c .. z23.c, z2.h, taken over from ../../src/cmd/popt by the port rules in ../HANDOFF_wega_disasm.md;
# spopt is the same source with -DSPOPT.
#
#   make                 build ./popt and ./spopt
#   test                 ../tools/corpus.sh --host popt spopt   (the corpora of ../../compare/popt and spopt, no make target)
#   make clean

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char

all: popt spopt

popt: z20.c z21.c z22.c z23.c z2.h
	$(HOSTCC) $(CFLAGS) -o $@ z20.c z21.c z22.c z23.c

spopt: z20.c z21.c z22.c z23.c z2.h
	$(HOSTCC) $(CFLAGS) -DSPOPT -o $@ z20.c z21.c z22.c z23.c

clean:
	rm -f popt spopt

.PHONY: all clean
