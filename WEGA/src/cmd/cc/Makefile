# cc: C compiler driver (WEGA 3.2 /bin/cc), decompiled from the original.
# Runs the passes cpp, cparse, gopt, codgen, popt, cas and ld; see cc.c.
#
#   make CC=/bin/cc CAS=/bin/cas LD=/bin/ld       on WEGA
#   make install PREFIX=dir                       install below dir

CC   ?= cc
CAS  ?= cas
LD   ?= ld
CFLAGS ?= -O
PREFIX ?= /
BINDIR ?= $(PREFIX)/bin
CRT0 ?= /lib/crt0.o
LIBC ?= /lib/libc.a

PROG = cc

all: $(PROG)

$(PROG): cc.o
	$(LD) -s -e start -o $@ $(CRT0) cc.o $(LIBC)

cc.o: cc.c
	$(CC) $(CFLAGS) -c cc.c

install: $(PROG)
	cp $(PROG) $(BINDIR)/$(PROG)
	chmod 755 $(BINDIR)/$(PROG)

clean:
	rm -f $(PROG) *.o

.PHONY: all install clean
