# ld, sld, gld: the WEGA loaders built for Linux (cross tools). They write the same files as /bin/ld
# (non-segmented), /bin/sld and /bin/gld (segmented) of WEGA. Sources: ld.c, sld.c, gld.c, written from the original
# binaries (see ../KNOWLEDGE.md).
#
#   make                 build ./ld, ./sld and ./gld
#   make check           compare with the original /bin/ld under z8k-emu
#                        (WEGA=<file tree of the original system>)

HOSTCC = cc
CFLAGS = -O2 -Wall -I../tools/lib
CONF   = ldconf.c ../tools/lib/conf.c
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)

all: ld sld gld

ld: ld.c $(CONF) ldconf.h
	$(HOSTCC) $(CFLAGS) -o $@ ld.c $(CONF)

sld: sld.c $(CONF) ldconf.h
	$(HOSTCC) $(CFLAGS) -o $@ sld.c $(CONF)

gld: gld.c $(CONF) ldconf.h
	$(HOSTCC) $(CFLAGS) -o $@ gld.c $(CONF)

check: ld sld gld
	WEGA=$(abspath $(WEGA)) EMU=$(EMU) python3 check.py

clean:
	rm -f ld sld gld

.PHONY: all check clean
