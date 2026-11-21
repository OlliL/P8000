# doscopy 1.0: DOS file commands (doscat, dosrm, doscp, dosdir, dosls,
# dosmkdir, dosrmdir). Source from the P8000 toolbox (usr/dos, May 1989),
# older than 1.1: no dosformat, no dosfat.c. Compiled with the WEGA cc,
# linked through the cc driver (crt0 and libc), stripped by install.
#
#   make                       build the DOS commands
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CC = cc
CFLAGS = -O
PREFIX = /
STRIP = strip

PROGS = doscat dosrm doscp dosdir dosls dosmkdir dosrmdir

OBJECTS = dosdev.o dosplit.o dosopn.o dosdel.o dosin.o dosout.o \
          doscat.o dosrm.o doscp.o dosdir.o dosls.o dosmkdir.o \
          dosrmdir.o
I = /usr/include

all: $(PROGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

$(OBJECTS): dos.h

doscat: doscat.o dosplit.o dosdev.o dosopn.o dosout.o
	$(CC) doscat.o dosplit.o dosdev.o dosopn.o dosout.o -o $@

dosrm: dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o
	$(CC) dosrm.o dosplit.o dosdev.o dosopn.o dosdel.o -o $@

doscp: doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosin.o dosout.o
	$(CC) doscp.o dosplit.o dosdev.o dosopn.o dosdel.o dosin.o \
	dosout.o -o $@

dosdir: dosdir.o dosplit.o dosdev.o dosopn.o
	$(CC) dosdir.o dosplit.o dosdev.o dosopn.o -o $@

dosls: dosls.o dosplit.o dosdev.o dosopn.o
	$(CC) dosls.o dosplit.o dosdev.o dosopn.o -o $@

dosmkdir: dosmkdir.o dosplit.o dosdev.o dosopn.o dosin.o dosdel.o
	$(CC) dosmkdir.o dosplit.o dosdev.o dosopn.o dosin.o dosdel.o \
	-o $@

dosrmdir: dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o
	$(CC) dosrmdir.o dosplit.o dosdev.o dosopn.o dosdel.o -o $@

install: $(PROGS)
	for p in $(PROGS); do cp $$p $(PREFIX)/usr/bin/$$p; $(STRIP) $(PREFIX)/usr/bin/$$p; done

clean:
	rm -f $(PROGS) $(OBJECTS)

dosdev.o:  dosdev.c
dosplit.o: dosplit.c $I/stdio.h $I/ctype.h
dosopn.o:  dosopn.c
dosdel.o:  dosdel.c
dosin.o:   dosin.c $I/time.h $I/sys/types.h $I/sys/stat.h
dosout.o:  dosout.c
doscat.o:  doscat.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosrm.o:   dosrm.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
doscp.o:   doscp.c $I/stdio.h $I/ctype.h $I/sys/types.h $I/sys/stat.h
dosdir.o:  dosdir.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosls.o:   dosls.c $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosmkdir.o: dosmkdir.c $I/time.h $I/stdio.h $I/sys/types.h $I/sys/stat.h
dosrmdir.o: dosrmdir.c $I/stdio.h $I/sys/types.h $I/sys/stat.h

.PHONY: all install clean
