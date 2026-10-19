# binutils: ranlib, strip and size of WEGA built for Linux (cross tools). They
# behave like the WEGA programs and write the same files. Sources: derived from
# ../../src/cmd/{ranlib,strip,size}.c; helpers in ../tools/lib (sobj, conf).
#
#   make                 build ranlib, strip, size
#   make check           compare with the originals under z8k-emu
#                        (WEGA=<file tree of the original system>)

HOSTCC = cc
CFLAGS = -O2 -Wall -Wno-unused-result
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)
LIB  = ../tools/lib

all: ranlib strip size

ranlib: ranlib.c $(LIB)/sobj.c $(LIB)/sobj.h $(LIB)/conf.c $(LIB)/conf.h
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ ranlib.c $(LIB)/sobj.c $(LIB)/conf.c

strip size: $(LIB)/sobj.c $(LIB)/sobj.h
strip: strip.c
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ strip.c $(LIB)/sobj.c
size: size.c
	$(HOSTCC) $(CFLAGS) -I$(LIB) -o $@ size.c $(LIB)/sobj.c

check: all ../ar/ar
	WEGA=$(abspath $(WEGA)) E=$(EMU) sh check.sh

../ar/ar:
	$(MAKE) -C ../ar

clean:
	rm -f ranlib strip size

.PHONY: all check clean
