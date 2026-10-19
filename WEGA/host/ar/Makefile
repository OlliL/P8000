# ar: the WEGA archive maintainer built for Linux (a cross tool: it behaves like
# /bin/ar of WEGA and writes the same archives). Source: ar.c, derived from
# WEGA/src/cmd/ar.c.
#
#   make                 build ./ar
#   make check           compare with the original /bin/ar under z8k-emu
#                        (WEGA=<file tree of the original system>)

HOSTCC = cc
CFLAGS = -O2 -Wall
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)

all: ar

LIB = ../tools/lib

ar: ar.c $(LIB)/wctime.c $(LIB)/wctime.h
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ ar.c $(LIB)/wctime.c

check: ar
	WEGA=$(abspath $(WEGA)) E=$(EMU) sh check.sh

clean:
	rm -f ar

.PHONY: all check clean
