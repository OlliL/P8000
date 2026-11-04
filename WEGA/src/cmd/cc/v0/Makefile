# ccv0: the driver of the old C compiler v0 (1982; host/OLDCC.md), a native WEGA program.
# It is ../cc.c compiled with -DOLD_V0: it runs the passes /lib/v0/cparse, /lib/v0/codgen and /lib/v0/popt (made
# in ../../cparse/v0, ../../codgen/v0, ../../popt/v0); cpp, gopt, cas, ld, crt0.o and libc.a are the shipped ones.
# 
#
#   make                   v0/ccv0
#   make install PREFIX=dir    dir/bin/ccv0 (the passes: make install in their v0 directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V0
LD = ld
PREFIX = /
LIBC = ../../../lib/libc-v8.a
all: ccv0

ccv0: cc.o
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(LIBC)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv0
	cp ccv0 $(PREFIX)/bin/ccv0

clean:
	rm -f ccv0 cc.o
