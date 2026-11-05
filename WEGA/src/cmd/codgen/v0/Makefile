# codgen v0: the code generator of the old C compiler v0 (1982: tight frames; it also built the passes of the shipped compiler), a native WEGA
# program. Made from the sources of .. with the switches of this version; the start-up file is that of .. (make .. oldprep first).
#
#   make             v0/codgen
#   make install PREFIX=dir    dir/lib/v0/codgen (the pass of ../../cc/v0/ccv0)
#   make clean
#
# Switches: ../oldcc.h (the version name only: the native cc takes about 160 characters of -D options).

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V0
PREFIX = /
LD = ld
LIBC = ../../../lib/libc-v5.a
CRT = ../
OBJS = bss3.o reader3.o rdr3.o local3.o order3.o match3.o allo3.o regallo3.o comm3.o wrt3.o regs3.o lcl3.o tr3.o optim3.o table3_codgen.o

all: codgen

codgen: $(OBJS)
	$(LD) -X -e start -i $(CRT)/crt0.o $(OBJS) $(LIBC) -o codgen

allo3.o: ../allo3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../allo3.c

wrt3.o: ../wrt3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../wrt3.c

local3.o: ../local3.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../hop_codgen.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../local3.c

match3.o: ../match3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../match3.c

order3.o: ../order3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../order3.c

regallo3.o: ../regallo3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regallo3.c

bss3.o: ../bss3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../bss3.c

reader3.o: ../reader3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../reader3.c

optim3.o: ../optim3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../optim3.c

comm3.o: ../comm3.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../ops_codgen.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../comm3.c

rdr3.o: ../rdr3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../rdr3.c

lcl3.o: ../lcl3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../lcl3.c

tr3.o: ../tr3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../tr3.c

regs3.o: ../regs3.c ../cg.h ../cg2.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regs3.c

table3_codgen.o: ../table3_codgen.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../table3_codgen.c

install: codgen
	-mkdir $(PREFIX)/lib/v0
	cp codgen $(PREFIX)/lib/v0/codgen

clean:
	rm -f codgen $(OBJS)
