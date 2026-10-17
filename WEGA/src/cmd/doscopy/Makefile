# doscopy: DOS file commands (doscat, dosrm, doscp, dosdir, dosls, dosmkdir,
# dosrmdir, dosformat). Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc), stripped at link time as in the original build.
#
#   make                       build the DOS commands
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CC ?= cc
CFLAGS ?= -O
PREFIX ?= /

PROGS = doscat dosrm doscp dosdir dosls dosmkdir dosrmdir dosformat

OBJECTS = dosdev.o dosplit.o dosopn.o dosdel.o dosfat.o dosin.o dosout.o \
          doscat.o dosrm.o doscp.o dosdir.o dosls.o dosmkdir.o \
          dosrmdir.o dosformat.o

all: $(PROGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJECTS): dos.h

doscat: doscat.o dosplit.o dosdev.o dosopn.o dosout.o
	$(CC) doscat.o dosplit.o dosdev.o dosopn.o dosout.o -o $@ -s

dosrm: dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o
	$(CC) dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o -o $@ -s

doscp: doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o dosout.o
	$(CC) doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o \
	dosout.o -o $@ -s

dosdir: dosdir.o dosplit.o dosdev.o dosopn.o
	$(CC) dosdir.o dosplit.o dosdev.o dosopn.o -o $@ -s

dosls: dosls.o dosplit.o dosdev.o dosopn.o
	$(CC) dosls.o dosplit.o dosdev.o dosopn.o -o $@ -s

dosmkdir: dosmkdir.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o
	$(CC) dosmkdir.o dosplit.o dosdev.o dosopn.o dosin.o dosdel.o \
	dosfat.o -o $@ -s

dosrmdir: dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o
	$(CC) dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o -o $@ -s

dosformat: dosformat.o dosplit.o
	$(CC) dosformat.o dosplit.o -o $@ -s

install: $(PROGS)
	for p in $(PROGS); do cp $$p $(PREFIX)/usr/bin/$$p; done

clean:
	rm -f $(PROGS) *.o

.PHONY: all install clean
