# lib: libc/build/nonseg/libc.a, libc/build/seg/slibc.a, libl/libl.a, libl/slibl.a, liby/liby.a, liby/sliby.a, libm/libm.a, libm/slibm.a, libPW/libPW.a, libPW/slibPW.a, libmp/libmp.a, libmp/slibmp.a, libtermlib/libtermlib.a, libtermlib/slibtermlib.a, libcurses/libcurses.a, libcurses/slibcurses.a, libscreen/libscreen.a, libscreen/slibscreen.a, libcisam/libcisam.a, libcisam/slibcisam.a, libu8000/libu8000.a, libu8000/slibu8000.a (the shipped libraries, from their sources), build/nonseg and build/seg: the loose objects of /lib and libc-vN.a (the libraries of the old programs, oldlibc/), libtermlib-v1.a (oldtermlib/).
#
#   make
#   make install PREFIX=dir   the shipped libraries and objects (cnvt.o, doprnt.o, mcrt0.o, scnvt.o, sdoprnt.o, smcrt0.o, libc.a, crt0.o, slibc.a, scrt0.o, libl.a, slibl.a, usr/lib/lex/ncform, liby.a, sliby.a, libm.a, slibm.a, usr/lib/libPW.a, usr/lib/slibPW.a, usr/lib/libmp.a, usr/lib/slibmp.a, usr/lib/libtermlib.a, usr/lib/slibtermlib.a, usr/lib/libcurses.a, usr/lib/slibcurses.a, usr/lib/libscreen.a, usr/lib/slibscreen.a, usr/lib/libcisam.a, usr/lib/slibcisam.a, usr/lib/libu8000.a, usr/lib/slibu8000.a) below dir
#   make clean
#
# The loose objects of /lib (build/nonseg, build/seg, from the sources in this directory): cnvt.o, doprnt.o (and scnvt.o,
# sdoprnt.o: not the members of libc.a; _hexchar for %X) by v1 without -O (ccv1, sccv1; doprnt with the stdio.h of
# src/head/oldv7), mcrt0.o and smcrt0.o (the start-up code for profiling, cc -p) by as (PLZ/ASM).

CC = ccv1
SCC = sccv1
AS = as
PREFIX = /
CFLAGS =
NOBJS = cnvt.o doprnt.o mcrt0.o
SOBJS = cnvt.o doprnt.o smcrt0.o

all: objs
	cd libc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libl ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd liby ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libm ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libPW ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libmp ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libtermlib ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libcurses ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libscreen ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libcisam ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libu8000 ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	# no make args, not threadsafe
	cd oldlibc ; $(MAKE)
	cd oldtermlib ; $(MAKE)
	cd oldslibc ; $(MAKE)

install: objs
	cp build/nonseg/cnvt.o $(PREFIX)/lib/cnvt.o
	cp build/nonseg/doprnt.o $(PREFIX)/lib/doprnt.o
	cp build/nonseg/mcrt0.o $(PREFIX)/lib/mcrt0.o
	cp build/seg/cnvt.o $(PREFIX)/lib/scnvt.o
	cp build/seg/doprnt.o $(PREFIX)/lib/sdoprnt.o
	cp build/seg/smcrt0.o $(PREFIX)/lib/smcrt0.o
	cd libc ; $(MAKE) install PREFIX=$(PREFIX)
	cd libl ; $(MAKE) install PREFIX=$(PREFIX)
	cd liby ; $(MAKE) install PREFIX=$(PREFIX)
	cd libm ; $(MAKE) install PREFIX=$(PREFIX)
	cd libPW ; $(MAKE) install PREFIX=$(PREFIX)
	cd libmp ; $(MAKE) install PREFIX=$(PREFIX)
	cd libtermlib ; $(MAKE) install PREFIX=$(PREFIX)
	cd libcurses ; $(MAKE) install PREFIX=$(PREFIX)
	cd libscreen ; $(MAKE) install PREFIX=$(PREFIX)
	cd libcisam ; $(MAKE) install PREFIX=$(PREFIX)
	cd libu8000 ; $(MAKE) install PREFIX=$(PREFIX)

clean:
	cd build/nonseg ; rm -f $(NOBJS)
	cd build/seg    ; rm -f $(SOBJS)
	cd libc ; $(MAKE) clean
	cd libl ; $(MAKE) clean
	cd liby ; $(MAKE) clean
	cd libm ; $(MAKE) clean
	cd libPW ; $(MAKE) clean
	cd libmp ; $(MAKE) clean
	cd libtermlib ; $(MAKE) clean
	cd libcurses ; $(MAKE) clean
	cd libscreen ; $(MAKE) clean
	cd libcisam ; $(MAKE) clean
	cd libu8000 ; $(MAKE) clean
	cd oldlibc ; $(MAKE) clean
	cd oldtermlib ; $(MAKE) clean
	cd oldslibc ; $(MAKE) clean

# the loose objects of /lib: NOBJS in build/nonseg, SOBJS in build/seg (installed as scnvt.o, sdoprnt.o, smcrt0.o)
objs: nonseg seg

nonseg: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile $(NOBJS)

seg: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) CFLAGS=-DSEG -f ../../Makefile $(SOBJS)

###################
# obj targets
###################

cnvt.o: ../../cnvt.c
	$(CC) $(CFLAGS) -c ../../cnvt.c

doprnt.o: ../../doprnt.c
	$(CC) $(CFLAGS) -I../../../head/oldv7 -c ../../doprnt.c

mcrt0.o: ../../mcrt0.s
	$(AS) -o mcrt0.o ../../mcrt0.s

smcrt0.o: ../../smcrt0.s
	$(AS) -o smcrt0.o ../../smcrt0.s

FORCE:

.PHONY: all objs nonseg seg install clean
