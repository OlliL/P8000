# doscopy: DOS file commands (doscat, dosrm, doscp, dosdir, dosls, dosmkdir,
# dosrmdir, dosformat). Compiled with the WEGA cc, linked through the cc
# driver (crt0 and libc), stripped by install.
#
#   make                       build the DOS commands
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CC = cc
CFLAGS = -O
PREFIX = /
STRIP = strip

PROGS = doscat dosrm doscp dosdir dosls dosmkdir dosrmdir dosformat

OBJECTS = dosdev.o dosplit.o dosopn.o dosdel.o dosfat.o dosin.o dosout.o \
          doscat.o dosrm.o doscp.o dosdir.o dosls.o dosmkdir.o \
          dosrmdir.o dosformat.o
I = /usr/include

all: $(PROGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJECTS): dos.h

doscat: doscat.o dosplit.o dosdev.o dosopn.o dosout.o
	$(CC) doscat.o dosplit.o dosdev.o dosopn.o dosout.o -o $@

dosrm: dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o
	$(CC) dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o -o $@

doscp: doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o dosout.o
	$(CC) doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosfat.o dosin.o \
	dosout.o -o $@

dosdir: dosdir.o dosplit.o dosdev.o dosopn.o
	$(CC) dosdir.o dosplit.o dosdev.o dosopn.o -o $@

dosls: dosls.o dosplit.o dosdev.o dosopn.o
	$(CC) dosls.o dosplit.o dosdev.o dosopn.o -o $@

dosmkdir: dosmkdir.o dosplit.o dosdev.o dosopn.o dosin.o dosdel.o dosfat.o
	$(CC) dosmkdir.o dosplit.o dosdev.o dosopn.o dosin.o dosdel.o \
	dosfat.o -o $@

dosrmdir: dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o
	$(CC) dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o -o $@

dosformat: dosformat.o dosplit.o
	$(CC) dosformat.o dosplit.o -o $@

install: $(PROGS)
	for p in $(PROGS); do cp $$p $(PREFIX)/usr/bin/$$p; $(STRIP) $(PREFIX)/usr/bin/$$p; done

clean:
	rm -f $(PROGS) $(OBJECTS)

dosdev.o:  dosdev.c dos.h
dosplit.o: dosplit.c dos.h
dosopn.o:  dosopn.c dos.h
dosdel.o:  dosdel.c dos.h
dosfat.o:  dosfat.c dos.h
dosin.o:   dosin.c dos.h
dosout.o:  dosout.c dos.h
doscat.o:  doscat.c dos.h
dosrm.o:   dosrm.c dos.h
doscp.o:   doscp.c dos.h
dosdir.o:  dosdir.c dos.h
dosls.o:   dosls.c dos.h
dosmkdir.o: dosmkdir.c dos.h
dosrmdir.o: dosrmdir.c dos.h
dosformat.o: dosformat.c dos.h

.PHONY: all install clean
