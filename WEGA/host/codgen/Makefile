# codgen, scodgen: the code generators of cc and scc built for Linux (cross tools: they behave like /lib/codgen and /lib/scodgen).
# Sources: taken over from ../../src/cmd/codgen by the port rules in ../HANDOFF_wega_disasm.md; scodgen is the same source with
# -DSEG and the tables table3_scodgen.c (ops_scodgen.c, hop_scodgen.c are included by comm3.c and local3.c).
#
#   make                 build ./codgen and ./scodgen
#   test                 ../tools/corpus.sh --host codgen scodgen   (the corpora of ../../compare/codgen and scodgen, no make target)
#   make clean

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char
COMMON = bss3.c reader3.c rdr3.c local3.c order3.c match3.c allo3.c regallo3.c comm3.c wrt3.c regs3.c lcl3.c tr3.c optim3.c
HDRS = cg.h cg2.h cgproto.h oldcc.h ops_codgen.c ops_scodgen.c hop_codgen.c hop_scodgen.c

all: codgen scodgen

codgen: $(COMMON) table3_codgen.c $(HDRS)
	$(HOSTCC) $(CFLAGS) -o $@ $(COMMON) table3_codgen.c

scodgen: $(COMMON) table3_scodgen.c $(HDRS)
	$(HOSTCC) $(CFLAGS) -DSEG -o $@ $(COMMON) table3_scodgen.c

clean:
	rm -f codgen scodgen

.PHONY: all clean
