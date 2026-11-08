# cparse, scparse: pass 1 of cc and scc (the PCC front end of WEGA) built for Linux (cross tools: they behave like /lib/cparse and /lib/scparse).
# Sources: taken over from ../../src/cmd/cparse by the port rules in ../HANDOFF_wega_disasm.md; scparse is the same source with -DSEG.
# cgram1.y is in the dialect of the host yacc (bison -y); the token numbers are fixed by the %term lines.
#
#   make                 build ./cparse and ./scparse
#   test                 ../tools/corpus.sh --host cparse scparse   (the corpora of ../../compare/cparse and scparse, no make target)
#   make clean

HOSTCC = cc
YACC = bison -y -Wno-yacc -Wno-other
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char
OBJS = xdefs1 scan1 pftn1 trees1 optim1 code1 local1 comm1 vinit1 sym1 sym2
SRCS = cgram1.c $(OBJS:=.c)
HDRS = mfile1 manifest macdefs cpproto.h

all: cparse scparse

cgram1.c: cgram1.y
	$(YACC) -o $@ cgram1.y

cparse: $(SRCS) $(HDRS)
	$(HOSTCC) $(CFLAGS) -I. -o $@ $(SRCS)

scparse: $(SRCS) $(HDRS)
	$(HOSTCC) $(CFLAGS) -I. -DSEG -o $@ $(SRCS)

clean:
	rm -f cparse scparse cgram1.c

.PHONY: all clean
