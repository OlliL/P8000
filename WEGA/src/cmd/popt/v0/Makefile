# popt v0: the peephole optimizer the old C compiler v0 (1982: tight frames; it also built the passes of the shipped compiler), a native WEGA
# program. Made from the sources of .. with the switches of this version; the start-up file is that of .. (make .. oldprep first).
#
#   make             v0/popt
#   make install PREFIX=dir    dir/lib/v0/popt (the pass of ../../cc/v0/ccv0)
#   make clean
#
# Switches (host/OLDCC.md):
#   (none yet: OLD_D20 is neutral on the test bench of v0)

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
	-mkdir $(PREFIX)/lib/v0
	cp popt $(PREFIX)/lib/v0/popt

clean:
	rm -f popt $(OBJS)
