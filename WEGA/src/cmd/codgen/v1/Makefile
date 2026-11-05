# codgen v1: the code generator of the old C compiler (the revision that built the WEGA binaries), a native WEGA
# program. Made from the sources of .. with the switches of this version; the start-up file is that of .. (make .. oldprep first).
#
#   make             v1/codgen
#   make install PREFIX=dir    dir/lib/v1/codgen (the pass of ../../cc/v1/ccv1)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D3    the registers keep their contents at the join of && and || (no rfreeall in cbranch)

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D3
PREFIX = /
LD = ld
LIBC = ../../../lib/libc-v5.a
CRT = ../
OBJS = bss3.o reader3.o rdr3.o local3.o order3.o match3.o allo3.o regallo3.o comm3.o wrt3.o regs3.o lcl3.o tr3.o optim3.o table3_codgen.o

SSRCS = bss3.c reader3.c rdr3.c local3.c order3.c match3.c allo3.c regallo3.c comm3.c wrt3.c regs3.c lcl3.c tr3.c optim3.c table3_scodgen.c
SOBJS = seg/bss3.o seg/reader3.o seg/rdr3.o seg/local3.o seg/order3.o seg/match3.o seg/allo3.o seg/regallo3.o seg/comm3.o seg/table3_scodgen.o seg/wrt3.o seg/regs3.o seg/lcl3.o seg/tr3.o seg/optim3.o

all: codgen scodgen

codgen: $(OBJS)
	$(LD) -X -e start -i $(CRT)/crt0.o $(OBJS) $(LIBC) -o codgen

scodgen: ../table3_scodgen.c $(CRT)/crt0.o
	-mkdir seg
	for f in $(SSRCS) ; do ( cd seg ; $(CC) $(CFLAGS) $(SWITCHES) -DSEG -c ../../$$f ) || exit 1 ; done
	$(LD) -X -e start -i $(CRT)/crt0.o $(SOBJS) $(LIBC) -o scodgen

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

install: codgen scodgen
	-mkdir $(PREFIX)/lib/v1
	cp codgen $(PREFIX)/lib/v1/codgen
	cp scodgen $(PREFIX)/lib/v1/scodgen

clean:
	rm -f codgen scodgen $(OBJS)
	rm -rf seg
