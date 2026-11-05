# lib: libc/libc.a (the shipped library, from its sources) and libc-vN.a (the libraries of the old programs, oldlibc/).
#
#   make
#   make clean

all:
	cd libc ; $(MAKE)
	cd oldlibc ; $(MAKE)
	cd oldslibc ; $(MAKE)

clean:
	cd libc ; $(MAKE) clean
	cd oldlibc ; $(MAKE) clean
	cd oldslibc ; $(MAKE) clean

.PHONY: all clean
