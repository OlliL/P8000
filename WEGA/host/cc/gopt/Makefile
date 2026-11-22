# gopt: the global optimizer of cc and scc (/lib/gopt) built for Linux (a cross tool: it behaves like /lib/gopt).
# Sources: taken over from ../../../src/cmd/cc/gopt by the port rules in ../../HANDOFF_wega_disasm.md; -DSHIPPED takes the forms of the
# source that do not depend on what the frame of the original holds (flow_graph, alloclpdef); ops_gopt.c is included by comm2.c.
#
#   make                 build ./gopt
#   test                 ../../tools/corpus.sh --host gopt   (the corpus of ../../../compare/gopt, no make target)
#   make clean

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char -DSHIPPED
SRCS = gdata.c reader2.c quads2.c debug2.c rdr2.c comm2.c wrtil2.c alias2.c flow2.c dom2.c init2.c live2.c ebbo2.c lpinv2.c lpind2.c beta2.c reg2.c alias2b.c
HDRS = mfile2.h goptproto.h ../z8000/macdefs ../mip/manifest ../mip/rdr.h ../mip/common.2.4 ../mip/rdr.2.4

all: gopt

gopt: $(SRCS) $(HDRS)
	$(HOSTCC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f gopt

.PHONY: all clean
