# uts: the WEGA kernel: conf (wega, boot, BOOTLIB, the start-up objects), sys (LIB1), dev (LIB2), h (the headers).
# Compiled with the WEGA scc (the kernel is segmented; the boot loader in conf with cc), assembled with as/cas, archived with ar,
# linked with sld (wega) and ld (boot). ver.c is made from ver.c+ by chkout (the rule .c+.c of make).
#
#   make                       build wega, boot and the libraries
#   make install PREFIX=dir    install below dir (default /); not yet part of "make install" in ../ (the kernel is not byte-identical yet)
#   make clean                 remove the objects and the results
#
# Order: sys and dev first (LIB1, LIB2), then conf links wega with them.

PREFIX = /
SUBDIRS = sys dev conf h

all:
	for d in $(SUBDIRS); do cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all || exit 1 ; cd .. ; done

install:
	for d in $(SUBDIRS); do cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) install PREFIX=$(PREFIX) || exit 1 ; cd .. ; done

clean:
	for d in $(SUBDIRS); do cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) clean ; cd .. ; done

.PHONY: all install clean
