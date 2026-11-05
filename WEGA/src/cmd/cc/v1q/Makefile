# ccv1q: the driver of the old C compiler v1q, a native WEGA program. It is ../cc.c compiled with -DOLD_V1Q: it runs the
# passes /lib/v1/cparse, /lib/v1q/codgen and /lib/v1/popt (../../cparse/v1, ../../codgen/v1q, ../../popt/v1).
# 
#
#   make                   v1q/ccv1q
#   make install PREFIX=dir    dir/bin/ccv1q (the passes: make install in their v1 directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V1Q
LD = ld
PREFIX = /
LIBC = ../../../lib/libc-v8.a
all: ccv1q

ccv1q: cc.o
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(LIBC)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv1q
	cp ccv1q $(PREFIX)/bin/ccv1q

clean:
	rm -f ccv1q cc.o
