# size: print the segment sizes of s.out files.
#
#   make CC=/bin/cc CAS=/bin/cas LD=/bin/ld       on WEGA
#   make install PREFIX=dir                       install below dir

CC   ?= cc
CAS  ?= cas
LD   ?= ld
CFLAGS ?= -O
PREFIX ?= /
BINDIR ?= $(PREFIX)/bin
LIBC ?= /lib/libc.a

PROG = size

all: $(PROG)

$(PROG): size.o crt0s.o
	$(LD) -s -e start -o $@ crt0s.o size.o $(LIBC)

size.o: size.c
	$(CC) $(CFLAGS) -c size.c

crt0s.o: crt0s.s
	$(CAS) -u -o $@ crt0s.s

install: $(PROG)
	cp $(PROG) $(BINDIR)/$(PROG)
	chmod 755 $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: all install clean
