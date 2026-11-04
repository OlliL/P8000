# cparse v0: pass 1 the old C compiler v0 (1982: tight frames; it also built the passes of the shipped compiler), a native WEGA program.
# Made from the sources of .. with the switches of this version; the start-up file, the bss layout and the libc
# part are those of .. (make .. first).
#
#   make             v0/cparse
#   make install PREFIX=dir    dir/lib/v0/cparse (the pass of ../../cc/v0/ccv0)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D12   the string literals of a file at the end of its data, one pool, aligned only at its start and end
#   OLD_D4R   no home in the frame for the register parameters in front of the first and behind the last other one
#   OLD_D6T   the values of a cpir switch go to codgen with its targets (record V), written together
#   OLD_D6C   up to 3 cases compared one after the other, the cpir table from 4 cases on

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D12 -DOLD_D4R -DOLD_D6T -DOLD_D6C -DOLD_D22
PREFIX = /
LD = ld
LIBC = ../../../lib/libc-v7.a
HDRS = ../mfile1 ../manifest ../macdefs ../stdio.h
OBJS = cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o sym2.o

all: cparse

cparse: ../crt0.o $(OBJS)
	$(LD) -i -e start ../crt0.o $(OBJS) $(LIBC) -o $@

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

install: cparse
	-mkdir $(PREFIX)/lib/v0
	cp cparse $(PREFIX)/lib/v0/cparse

clean:
	rm -f cparse *.o
