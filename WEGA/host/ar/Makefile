# ar: the WEGA archive maintainer built for Linux (a cross tool: it behaves like
# /bin/ar of WEGA and writes the same archives). Source: ar.c, derived from
# WEGA/src/cmd/ar.c (port rules: ../HANDOFF_wega_disasm.md).
#
#   make                 build ./ar
#   make check           corpus (progcfg.py, scenarios.sh) against the original /bin/ar under z8k-emu
#                        (WEGA=<file tree of the original system>)

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra
WEGA = ../../../tmp/wega31_root

all: ar

LIB = ../tools/lib

ar: ar.c $(LIB)/wctime.c $(LIB)/wctime.h
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ ar.c $(LIB)/wctime.c

check: ar
	WEGA_ROOT=$(abspath $(WEGA)) sh ../tools/corpus.sh --host ar $(CURDIR)

clean:
	rm -f ar

.PHONY: all check clean
