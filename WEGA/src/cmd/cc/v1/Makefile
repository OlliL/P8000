# ccv1: the driver of the old C compiler v1 (the revision that built the WEGA binaries), a native WEGA program.
# It is ../cc.c compiled with -DOLD_V1: it runs the passes /lib/v1/cparse, /lib/v1/codgen and /lib/v1/popt (made
# in ../../cparse/v1, ../../codgen/v1, ../../popt/v1); cpp, gopt, cas, ld, crt0.o and libc.a are the shipped ones.
# 
#
#   make                   v1/ccv1
#   make install PREFIX=dir    dir/bin/ccv1 (the passes: make install in their v1 directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V1
LD = ld
PREFIX = /
LIBC = ../../../lib/libc-v8.a
all: ccv1

ccv1: cc.o
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(LIBC)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv1
	cp ccv1 $(PREFIX)/bin/ccv1

clean:
	rm -f ccv1 cc.o
