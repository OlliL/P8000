# codgen v1q: the code generator of the old C compiler v1q (older than v1: the libc linked into size), a native WEGA
# program. Made from the sources of .. with the switches of this version; the start-up file is that of .. (make .. oldprep first).
#
#   make             v1q/codgen
#   make install PREFIX=dir    dir/lib/v1q/codgen (the pass of ../../cc/v1q/ccv1q)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D3    the registers keep their contents at the join of && and || (no rfreeall in cbranch)
#   OLD_D14Q  the value of a ?: goes through a temporary of the parameter area
#   OLD_D1S   no temporary for a return of a constant or a call

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D3 -DOLD_D14Q -DOLD_D1S
PREFIX = /
LD = ld
LIBC = ../../../lib/libc-v5.a
CRT = ../../../lib/libc/crt
OBJS = bss3.o reader3.o rdr3.o local3.o order3.o match3.o allo3.o regallo3.o comm3.o wrt3.o regs3.o lcl3.o tr3.o optim3.o table3_codgen.o

all: codgen

codgen: $(OBJS)
	$(LD) -X -e start -i $(CRT)/crt0-codgen.o $(OBJS) $(LIBC) -o codgen

allo3.o: ../allo3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../allo3.c

wrt3.o: ../wrt3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../wrt3.c

local3.o: ../local3.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../local3.c

match3.o: ../match3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../match3.c

order3.o: ../order3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../order3.c

regallo3.o: ../regallo3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regallo3.c

bss3.o: ../bss3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../bss3.c

reader3.o: ../reader3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../reader3.c

optim3.o: ../optim3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../optim3.c

comm3.o: ../comm3.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../comm3.c

rdr3.o: ../rdr3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../rdr3.c

lcl3.o: ../lcl3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../lcl3.c

tr3.o: ../tr3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../tr3.c

regs3.o: ../regs3.c ../cg.h ../cg2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regs3.c

table3_codgen.o: ../table3_codgen.c ../cg.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../table3_codgen.c

install: codgen
	-mkdir $(PREFIX)/lib/v1q
	cp codgen $(PREFIX)/lib/v1q/codgen

clean:
	rm -f codgen $(OBJS)
