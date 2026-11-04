# ccv0p: the driver of the old C compiler v0p (the compiler of the passes; host/OLDCC.md), a native WEGA program.
# It is ../cc.c compiled with -DOLD_V0P: it runs the passes /lib/v0p/cparse, /lib/v0p/codgen and /lib/v0p/popt (made
# in ../../cparse/v0p, ../../codgen/v0p, ../../popt/v0p); cpp, gopt, cas, ld, crt0.o and libc.a are the shipped ones.
# 
#
#   make                   v0p/ccv0p
#   make install PREFIX=dir    dir/bin/ccv0p (the passes: make install in their v0p directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V0P
LD = ld
PREFIX = /
LIBC = ../../../lib/libc-v8.a
all: ccv0p

ccv0p: cc.o
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(LIBC)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv0p
	cp ccv0p $(PREFIX)/bin/ccv0p

clean:
	rm -f ccv0p cc.o
