# lib: libc/libc.a (the shipped library, from its sources) and libc-vN.a (the libraries of the old programs, oldlibc/).
#
#   make
#   make install PREFIX=dir   the shipped library (libc.a, crt0.o) below dir
#   make clean

PREFIX = /

all:
	cd libc ; $(MAKE)
	cd oldlibc ; $(MAKE)
	cd oldslibc ; $(MAKE)

install:
	cd libc ; $(MAKE) install PREFIX=$(PREFIX)

clean:
	cd libc ; $(MAKE) clean
	cd oldlibc ; $(MAKE) clean
	cd oldslibc ; $(MAKE) clean

.PHONY: all install clean
