# lib: libc/libc.a and slibc/slibc.a (the shipped libraries, from their sources) and libc-vN.a (the libraries of the old programs, oldlibc/).
#
#   make
#   make install PREFIX=dir   the shipped libraries (libc.a, crt0.o, slibc.a, scrt0.o) below dir
#   make clean

PREFIX = /

all:
	cd libc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	cd slibc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS)
	# no make args, not threadsafe
	cd oldlibc ; $(MAKE)
	cd oldslibc ; $(MAKE)

install:
	cd libc ; $(MAKE) install PREFIX=$(PREFIX)
	cd slibc ; $(MAKE) install PREFIX=$(PREFIX)

clean:
	cd libc ; $(MAKE) clean
	cd slibc ; $(MAKE) clean
	cd oldlibc ; $(MAKE) clean
	cd oldslibc ; $(MAKE) clean

.PHONY: all install clean
