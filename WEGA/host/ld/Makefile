# ld, sld, gld: the WEGA loaders built for Linux (cross tools). They write the same files as /bin/ld
# (non-segmented), /bin/sld and /bin/gld (segmented) of WEGA. Sources: ld.c, sld.c, gld.c, written from the original
# binaries (see ../KNOWLEDGE.md).
#
#   make                 build ./ld, ./sld and ./gld

HOSTCC = cc
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -I../tools/lib
CONF   = ldconf.c ../tools/lib/conf.c
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)

all: ld sld gld

LDSRC = nonseg/ld.c nonseg/symbols.c nonseg/copyname.c nonseg/printname.c

ld: $(LDSRC) nonseg/ld.h nonseg/proto.h $(CONF) ldconf.h
	$(HOSTCC) $(CFLAGS) -fsigned-char -Inonseg -I. -o $@ $(LDSRC) $(CONF)

SLDSRC = seg/ld.c seg/ldm.c seg/symbols.c seg/copyname.c seg/printname.c seg/swap.c seg/ext.c seg/dump.c

sld: $(SLDSRC) seg/ld.h seg/proto.h $(CONF) ldconf.h
	$(HOSTCC) $(CFLAGS) -fsigned-char -Iseg -I. -o $@ $(SLDSRC) $(CONF)

gld: gld.c $(CONF) ldconf.h
	$(HOSTCC) $(CFLAGS) -o $@ gld.c $(CONF)

clean:
	rm -f ld sld gld

.PHONY: all clean
