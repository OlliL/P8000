# codgen, scodgen: the code generators of cc and scc built for Linux (cross tools: they behave like /lib/codgen and /lib/scodgen).
# Sources: taken over from ../../../src/cmd/cc/codgen by the port rules in ../../HANDOFF_wega_disasm.md; scodgen is the same source with
# -DSEG (table3.c then includes table3.szcc.c; local3.c includes hop_scodgen.c).
#
#   make                 build ./codgen and ./scodgen
#   test                 ../../tools/corpus.sh --host codgen scodgen   (the corpora of ../../../compare/codgen and scodgen, no make target)
#   make clean

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char
COMMON = bss3.c reader3.c rdr3.c local3.c order3.c match3.c allo3.c regallo3.c comm3.c table3.c wrt3.c regs3.c lcl3.c tr3.c optim3.c
HDRS = mfile3 cgproto.h oldcc.h ../z8000/macdefs ../mip/manifest ../mip/rdr.h ../mip/common.2.5 ../mip/optim.2.5 ../mip/rdr.2.5 hop_codgen.c hop_scodgen.c table3.nzcc.c table3.szcc.c

all: codgen scodgen

codgen: $(COMMON) $(HDRS)
	$(HOSTCC) $(CFLAGS) -o $@ $(COMMON)

scodgen: $(COMMON) $(HDRS)
	$(HOSTCC) $(CFLAGS) -DSEG -o $@ $(COMMON)

clean:
	rm -f codgen scodgen

.PHONY: all clean
