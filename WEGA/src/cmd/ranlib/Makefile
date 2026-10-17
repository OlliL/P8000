# ranlib: archive index tool. Work in progress: ranlib.c is not yet
# byte-identical with the original /bin/ranlib (needs the older cc, see
# ../../TODO.md).
#
#   make CC=/bin/cc CAS=/bin/cas LD=/bin/ld       on WEGA
#   make install PREFIX=dir                       install below dir

CC   ?= cc
LD   ?= ld
CFLAGS ?= -O
PREFIX ?= /
BINDIR ?= $(PREFIX)/bin
CRT0 ?= /lib/crt0.o
LIBC ?= /lib/libc.a

PROG = ranlib

all: $(PROG)

$(PROG): ranlib.o
	$(LD) -s -e start -o $@ $(CRT0) ranlib.o $(LIBC)

ranlib.o: ranlib.c
	$(CC) $(CFLAGS) -c ranlib.c

install: $(PROG)
	cp $(PROG) $(BINDIR)/$(PROG)
	chmod 755 $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: all install clean
