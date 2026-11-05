# cc and scc, the drivers. cc.c is the source of both: scc is cc.c compiled with -DSCC. Byte-identical with the originals.
#
#   make              build ./cc and ./scc
#   make old          the drivers of the old compiler versions in OLDDIRS (v1/Makefile: ccv1)
#   make install PREFIX=dir
#   make clean

CC = cc
LD = ld
CFLAGS = -O
PREFIX = /
LIBC = ../../lib/libc-v8.a
CRT0 = /lib/crt0.o
OLDDIRS = v1 v0 v0p v1q v1r

all: cc scc

old:
	for d in $(OLDDIRS); do cd $$d ; $(MAKE) ; cd .. ; done

cc: cc.o $(LIBC)
	$(LD) -s -e start -o $@ $(CRT0) cc.o $(LIBC)

scc: scc.o $(LIBC)
	$(LD) -s -e start -o $@ $(CRT0) scc.o $(LIBC)

cc.o: cc.c
	$(CC) $(CFLAGS) -c cc.c

scc.o: cc.c
	cp cc.c scc.c
	$(CC) $(CFLAGS) -DSCC -c scc.c
	rm -f scc.c

install: cc scc
	cp cc $(PREFIX)/bin/cc
	cp scc $(PREFIX)/bin/scc

clean:
	rm -f *.o cc scc
	for d in $(OLDDIRS); do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all old install clean
