#
# makefile for libc/port
#
SHELL=/bin/sh


all:
	cd gen; make
	cd print; make
	cd stdio; make

clean clobber:
	-rm -f */*.o
