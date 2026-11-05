# cparse v1: pass 1 of the old C compiler (the revision that built the WEGA binaries), a native WEGA program.
# Made from the sources of .. with the switches of this version; the start-up file, the bss layout and the libc
# part are those of .. (make .. first).
#
#   make             v1/cparse
#   make install PREFIX=dir    dir/lib/v1/cparse (the pass of ../../cc/v1/ccv1)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D12   the string literals of a file at the end of its data, one pool, aligned only at its start and end
#   OLD_D1    a register variable gets its place in the frame too (at its declaration, like an automatic variable)

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D12 -DOLD_D1
PREFIX = /
LD = ld
LIBC = ../../../lib/libc-v7.a
HDRS = ../mfile1 ../manifest ../macdefs ../stdio.h
OBJS = cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o sym2.o
SOBJS = seg/cgram1.o seg/xdefs1.o seg/scan1.o seg/pftn1.o seg/trees1.o seg/optim1.o seg/code1.o seg/local1.o seg/comm1.o seg/vinit1.o seg/sym1.o seg/sym2.o

all: cparse

cparse: ../crt0.o $(OBJS)
	$(LD) -i -e start ../crt0.o $(OBJS) $(LIBC) -o $@

scparse: ../crt0.o
	-mkdir seg
	for f in cgram1 xdefs1 scan1 pftn1 trees1 optim1 code1 local1 comm1 vinit1 sym1 sym2 ; do ( cd seg ; $(CC) $(CFLAGS) $(SWITCHES) -DSEG -c ../../$$f.c ) || exit 1 ; done
	$(LD) -i -e start ../crt0.o $(SOBJS) $(LIBC) -o $@

cgram1.o: ../cgram1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cgram1.c

xdefs1.o: ../xdefs1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../xdefs1.c

scan1.o: ../scan1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../scan1.c

pftn1.o: ../pftn1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../pftn1.c

trees1.o: ../trees1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../trees1.c

optim1.o: ../optim1.c $(HDRS) ../optim
	$(CC) $(CFLAGS) $(SWITCHES) -c ../optim1.c

code1.o: ../code1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../code1.c

local1.o: ../local1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../local1.c

comm1.o: ../comm1.c $(HDRS) ../common
	$(CC) $(CFLAGS) $(SWITCHES) -c ../comm1.c

vinit1.o: ../vinit1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../vinit1.c

sym1.o: ../sym1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../sym1.c
sym2.o: ../sym2.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../sym2.c

install: cparse scparse
	-mkdir $(PREFIX)/lib/v1
	cp cparse $(PREFIX)/lib/v1/cparse
	cp scparse $(PREFIX)/lib/v1/scparse

clean:
	rm -f cparse *.o scparse
	rm -rf seg
