# popt: peephole optimizer pass of the WEGA C compiler (/lib/popt).
# z20.c tables, z21.c main/flow, z22.c register tracking, z23.c liveness and
# register renaming. NOTES.md describes the reproduced quirks.
#
#   make CC=/bin/cc LD=/bin/ld                    on WEGA
#   make install PREFIX=dir                       install below dir

CC   ?= cc
LD   ?= ld
CFLAGS ?= -O
PREFIX ?= /
LIBDIR ?= $(PREFIX)/lib
CRT0 ?= /lib/crt0.o
LIBC ?= /lib/libc.a

PROG = popt
OBJS = z20.o z21.o z22.o z23.o

all: $(PROG)

$(PROG): $(OBJS)
	$(LD) -s -e start -o $@ $(CRT0) $(OBJS) $(LIBC)

z20.o: z20.c z2.h
	$(CC) $(CFLAGS) -c z20.c
z21.o: z21.c z2.h
	$(CC) $(CFLAGS) -c z21.c
z22.o: z22.c z2.h
	$(CC) $(CFLAGS) -c z22.c
z23.o: z23.c z2.h
	$(CC) $(CFLAGS) -c z23.c

install: $(PROG)
	cp $(PROG) $(LIBDIR)/$(PROG)
	chmod 755 $(LIBDIR)/$(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: all install clean
