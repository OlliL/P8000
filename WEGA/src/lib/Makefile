# lib: libc/libc.a, slibc/slibc.a, libl/libl.a, libl/slibl.a (the shipped libraries, from their sources) and libc-vN.a (the libraries of the old programs, oldlibc/).
#
#   make
#   make install PREFIX=dir   the shipped libraries (libc.a, crt0.o, slibc.a, scrt0.o, libl.a, slibl.a, usr/lib/lex/ncform) below dir
#   make clean

PREFIX = /

all:
	cd libc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd slibc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
#	cd libl ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	# no make args, not threadsafe
	cd oldlibc ; $(MAKE)
	cd oldslibc ; $(MAKE)

install:
	cd libc ; $(MAKE) install PREFIX=$(PREFIX)
	cd slibc ; $(MAKE) install PREFIX=$(PREFIX)
#	cd libl ; $(MAKE) install PREFIX=$(PREFIX)

clean:
	cd libc ; $(MAKE) clean
	cd slibc ; $(MAKE) clean
#	cd libl ; $(MAKE) clean
	cd oldlibc ; $(MAKE) clean
	cd oldslibc ; $(MAKE) clean

.PHONY: all install clean
