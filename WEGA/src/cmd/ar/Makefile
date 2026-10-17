# ar: archive maintainer (System III/V7 ar 2.2, WEGA 3.2 /bin/ar).
#
# crt0v.s is the start-up code variant of this binary (not the standard crt0).
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

PROG = ar

all: $(PROG)

$(PROG): ar.o crt0v.o
	$(LD) -s -e start -o $@ crt0v.o ar.o $(LIBC)

ar.o: ar.c
	$(CC) $(CFLAGS) -c ar.c

crt0v.o: crt0v.s
	$(CAS) -u -o $@ crt0v.s

install: $(PROG)
	cp $(PROG) $(BINDIR)/$(PROG)
	chmod 755 $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: all install clean
