# cparse, scparse: pass 1 of cc and scc (the PCC front end of WEGA) built for Linux (cross tools: they behave like /lib/cparse and /lib/scparse).
# Sources: taken over from ../../../src/cmd/cc/cparse by the port rules in ../../HANDOFF_wega_disasm.md; scparse is the same source with -DSEG.
# cgram1.y is in the dialect of the host yacc (bison -y); the token numbers are fixed by the %term lines.
#
#   make                 build ./cparse and ./scparse
#   test                 ../../tools/corpus.sh --host cparse scparse   (the corpora of ../../../compare/cparse and scparse, no make target)
#   make clean

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char
UNAME = $(shell uname -s)
ifeq ($(UNAME),Darwin)
# the bison 2.3 of macOS has no -W options and no %define api.token.prefix: the grammar
# goes without that line and the token enum and the YYSTYPE typedef (both are in mfile1) are switched off
YACC = bison -y
YYFIX = tail -n +2 cgram1.y > cgram1.mac.y && $(YACC) -o $@ cgram1.mac.y && rm -f cgram1.mac.y
CFLAGS += -DYYTOKENTYPE -DYYSTYPE_IS_DECLARED -DYYSTYPE_IS_TRIVIAL=1
else
YACC = bison -y -Wno-yacc -Wno-other
YYFIX = $(YACC) -o $@ cgram1.y
endif
OBJS = xdefs1 scan1 pftn1 trees1 optim1 code1 local1 comm1 vinit1 sym1 sym2 zalloc
SRCS = cgram1.c $(OBJS:=.c)
HDRS = mfile1 ../mip/manifest ../z8000/macdefs cpproto.h ../mip/common.2.5 ../mip/optim.2.5

all: cparse scparse

cgram1.c: cgram1.y
	$(YYFIX)

cparse: $(SRCS) $(HDRS)
	$(HOSTCC) $(CFLAGS) -I. -o $@ $(SRCS)

scparse: $(SRCS) $(HDRS)
	$(HOSTCC) $(CFLAGS) -I. -DSEG -o $@ $(SRCS)

clean:
	rm -f cparse scparse cgram1.c

.PHONY: all clean
