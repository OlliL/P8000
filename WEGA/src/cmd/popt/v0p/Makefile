# popt v0p: the peephole optimizer the old C compiler v0p (the compiler of the passes cparse, codgen, popt: v0 of 1982 with the differences of the passes), a native WEGA
# program. Made from the sources of .. with the switches of this version; the start-up file is that of .. (make .. oldprep first).
#
#   make             v0p/popt
#   make install PREFIX=dir    dir/lib/v0p/popt (the pass of ../../cc/v0p/ccv0p)
#   make clean
#
# No switches: the source of .. as it is (host/OLDCC.md, D20).

CC = cc
CFLAGS = -O
SWITCHES =
PREFIX = /
LD = ld
LIBC = ../../../lib/libc-v5.a
OBJS = z20.o z21.o z22.o z23.o

all: popt

popt: $(OBJS)
	$(LD) -X -e start -i /lib/crt0.o $(OBJS) $(LIBC) -o popt

z20.o: ../z20.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z20.c

z21.o: ../z21.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z21.c

z22.o: ../z22.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z22.c

z23.o: ../z23.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z23.c

install: popt
	-mkdir $(PREFIX)/lib/v0p
	cp popt $(PREFIX)/lib/v0p/popt

clean:
	rm -f popt $(OBJS)
