# popt v1: the peephole optimizer of the old C compiler (the revision that built the WEGA binaries), a native WEGA
# program. Made from the sources of .. with the switches of this version; the start-up file is that of .. (make .. oldprep first).
#
#   make             v1/popt
#   make install PREFIX=dir    dir/lib/v1/popt (the pass of ../../cc/v1/ccv1)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D20   code motion (codemove): a jump to the next label does not end a block

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D20
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
	-mkdir $(PREFIX)/lib/v1
	cp popt $(PREFIX)/lib/v1/popt

clean:
	rm -f popt $(OBJS)
