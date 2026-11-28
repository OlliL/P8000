# sh: /bin/sh (and its link /bin/rsh) from the Bourne shell of System III (usr/src/cmd/sh) as WEGA changed it (SCCS lines
# 08/06/83 .. 08/22/83): the builtins unset (a name leaves the tree of names) and ESC B (empty), a script is run by the
# shell its first line names (#!), a login flag of argv[0], setret/longret for setjmp; profile.c (monitor) of WEGA.
# ccv1, separate I and D (-i), the files in alphabetical order with test.c last (sh.mk); service.c (08/22/83, padded
# strings, register slots) by ccvm (CCS). libc-v9 (extended for sh, owner 2026-11-26). Open: compare/sh/findings.json.
#
#   make              build ./sh
#   make install PREFIX=dir    (bin/sh and its link bin/rsh)
#   make clean

CC = ccv1
CCS = ccvm
CAS = cas
LD = ld
CFLAGS = -O -I../../head/oldv7
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v9.a
CRT = ../../lib/libc/nonseg/crt

OBJECTS = args.o blok.o builtin.o cmd.o ctype.o error.o expand.o fault.o io.o macro.o main.o msg.o name.o print.o \
	profile.o service.o setbrk.o stak.o string.o word.o xec.o test.o

all: sh

sh: crt0.o $(OBJECTS) $(LIBC)
	$(LD) -i -e start -o $@ crt0.o $(OBJECTS) $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh sh > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

$(OBJECTS): defs.h mac.h mode.h name.h stak.h brkincr.h ctype.h

.c.o:
	$(CC) $(CFLAGS) -c $<

service.o: service.c
	$(CCS) $(CFLAGS) -c service.c

install: sh
	cp sh $(PREFIX)/bin/sh
	$(STRIP) $(PREFIX)/bin/sh
	rm -f $(PREFIX)/bin/rsh
	ln $(PREFIX)/bin/sh $(PREFIX)/bin/rsh

clean:
	rm -f *.o crt0.s sh

.PHONY: all install clean


# the library of /lib is not made here (the shipped build)
/lib/libc.a:
