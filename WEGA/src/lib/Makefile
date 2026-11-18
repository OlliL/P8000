# lib: libc/build/nonseg/libc.a, libc/build/seg/slibc.a, libl/libl.a, libl/slibl.a, liby/liby.a, liby/sliby.a, libm/libm.a, libm/slibm.a, libPW/libPW.a, libPW/slibPW.a, libmp/libmp.a, libmp/slibmp.a, libtermlib/libtermlib.a, libtermlib/slibtermlib.a, libcurses/libcurses.a, libcurses/slibcurses.a, libscreen/libscreen.a, libscreen/slibscreen.a, libu8000/libu8000.a, libu8000/slibu8000.a (the shipped libraries, from their sources) and libc-vN.a (the libraries of the old programs, oldlibc/).
#
#   make
#   make install PREFIX=dir   the shipped libraries (libc.a, crt0.o, slibc.a, scrt0.o, libl.a, slibl.a, usr/lib/lex/ncform, liby.a, sliby.a, libm.a, slibm.a, usr/lib/libPW.a, usr/lib/slibPW.a, usr/lib/libmp.a, usr/lib/slibmp.a, usr/lib/libtermlib.a, usr/lib/slibtermlib.a, usr/lib/libcurses.a, usr/lib/slibcurses.a, usr/lib/libscreen.a, usr/lib/slibscreen.a, usr/lib/libu8000.a, usr/lib/slibu8000.a) below dir
#   make clean

PREFIX = /

all:
	cd libc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libl ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd liby ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libm ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libPW ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libmp ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libtermlib ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libcurses ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libscreen ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd libu8000 ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	# no make args, not threadsafe
	cd oldlibc ; $(MAKE)
	cd oldslibc ; $(MAKE)

install:
	cd libc ; $(MAKE) install PREFIX=$(PREFIX)
	cd libl ; $(MAKE) install PREFIX=$(PREFIX)
	cd liby ; $(MAKE) install PREFIX=$(PREFIX)
	cd libm ; $(MAKE) install PREFIX=$(PREFIX)
	cd libPW ; $(MAKE) install PREFIX=$(PREFIX)
	cd libmp ; $(MAKE) install PREFIX=$(PREFIX)
	cd libtermlib ; $(MAKE) install PREFIX=$(PREFIX)
	cd libcurses ; $(MAKE) install PREFIX=$(PREFIX)
	cd libscreen ; $(MAKE) install PREFIX=$(PREFIX)
	cd libu8000 ; $(MAKE) install PREFIX=$(PREFIX)

clean:
	cd libc ; $(MAKE) clean
	cd libl ; $(MAKE) clean
	cd liby ; $(MAKE) clean
	cd libm ; $(MAKE) clean
	cd libPW ; $(MAKE) clean
	cd libmp ; $(MAKE) clean
	cd libtermlib ; $(MAKE) clean
	cd libcurses ; $(MAKE) clean
	cd libscreen ; $(MAKE) clean
	cd libu8000 ; $(MAKE) clean
	cd oldlibc ; $(MAKE) clean
	cd oldslibc ; $(MAKE) clean

.PHONY: all install clean
