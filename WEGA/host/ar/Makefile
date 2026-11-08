# ar: the WEGA archive maintainer built for Linux (a cross tool: it behaves like
# /bin/ar of WEGA and writes the same archives). Source: ar.c, derived from
# WEGA/src/cmd/ar.c (port rules: ../HANDOFF_wega_disasm.md).
#
#   make                 build ./ar
#   test                 ../tools/corpus.sh --host ar   (the corpus of ../../compare/ar, no make target)

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra

all: ar

LIB = ../tools/lib

ar: ar.c $(LIB)/wctime.c $(LIB)/wctime.h
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ ar.c $(LIB)/wctime.c

clean:
	rm -f ar

.PHONY: all clean
