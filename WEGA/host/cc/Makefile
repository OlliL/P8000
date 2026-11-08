# cc: the WEGA C compiler driver built for Linux (a cross tool: it behaves
# like /bin/cc of WEGA; it runs the passes, which are the WEGA programs run
# under z8k-emu for now). Source: cc.c, taken over from ../../src/cmd/cc by the port rules in ../HANDOFF_wega_disasm.md.
#
#   make                 build ./cc and ./scc (the same source, scc with -DSCC)
#   test                 ../tools/corpus.sh --host cc scc   (the corpora of ../../compare/cc and scc, no make target)

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99

all: cc scc

LIB  = ../tools/lib

cc: cc.c $(LIB)/conf.c $(LIB)/conf.h
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ cc.c $(LIB)/conf.c

scc: cc.c $(LIB)/conf.c $(LIB)/conf.h
	$(HOSTCC) $(CFLAGS) -DSCC -I$(LIB) -o $@ cc.c $(LIB)/conf.c

clean:
	rm -f cc scc

.PHONY: all clean
