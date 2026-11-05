# ccv1r: the driver of the old C compiler v1r, a native WEGA program. It is ../cc.c compiled with -DOLD_V1R: it runs the
# passes /lib/v1/cparse, /lib/v1r/codgen and /lib/v1/popt (../../cparse/v1, ../../codgen/v1r, ../../popt/v1).
# 
#
#   make                   v1r/ccv1r
#   make install PREFIX=dir    dir/bin/ccv1r (the passes: make install in their v1 directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V1R
LD = ld
PREFIX = /
LIBC = ../../../lib/libc-v8.a
all: ccv1r

ccv1r: cc.o
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(LIBC)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv1r
	cp ccv1r $(PREFIX)/bin/ccv1r

clean:
	rm -f ccv1r cc.o
