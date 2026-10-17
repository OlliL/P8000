# strip: remove symbols/relocation (and with -h the header) from s.out files.
#
#   make CC=/bin/cc CAS=/bin/cas LD=/bin/ld       on WEGA
#   make install PREFIX=dir                       install below dir

CC   ?= cc
CAS  ?= cas
LD   ?= ld
CFLAGS ?= -O
PREFIX ?= /
BINDIR ?= $(PREFIX)/usr/bin
LIBC ?= /lib/libc.a

PROG = strip

all: $(PROG)

$(PROG): strip.o crt0s.o
	$(LD) -s -e start -o $@ crt0s.o strip.o $(LIBC)

strip.o: strip.c
	$(CC) $(CFLAGS) -c strip.c

crt0s.o: crt0s.s
	$(CAS) -u -o $@ crt0s.s

install: $(PROG)
	cp $(PROG) $(BINDIR)/$(PROG)
	chmod 755 $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: all install clean
