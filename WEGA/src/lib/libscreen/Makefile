# libscreen: /usr/lib/libscreen.a and /usr/lib/slibscreen.a (the "screen-oriented" interface over curses of vls and vnews,
# nonsegmented and segmented). No source is known (System III, BSD, the web, the local backups): io.c, mesg.c, mvcursor.c
# and scrn.c are written from the original members. Both libraries are made from the same sources (Library.SC 01/30/83) by
# v1 (ccv1, sccv1 from OLDROOT) with -O; io.c needs the V7 stdio.h (src/head/oldv7: getchar). compare/libscreen/SYSIII.md.
# Headers: arhdr_libscreen.s, arhdr_slibscreen.s, mkar.sh.
#
#   make        libscreen.a slibscreen.a
#   make install PREFIX=dir   dir/usr/lib/libscreen.a, dir/usr/lib/slibscreen.a
#   make clean

CC = ccv1
SCC = sccv1
CAS = cas
PREFIX = /
CFLAGS = -O -I../../../../head/oldv7
OBJS = io.o mesg.o mvcursor.o scrn.o

all: build/nonseg/libscreen.a build/seg/slibscreen.a

build/nonseg/libscreen.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile libscreen.a

build/seg/slibscreen.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) -f ../../Makefile slibscreen.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libscreen.a $(PREFIX)/usr/lib/libscreen.a
	cp build/seg/slibscreen.a $(PREFIX)/usr/lib/slibscreen.a

clean:
	cd build/nonseg ; rm -f libscreen.a  arhdr.o $(OBJS) Library.SC
	cd build/seg    ; rm -f slibscreen.a arhdr.o $(OBJS) Library.SC

##################
# bin targets
##################

libscreen.a: $(OBJS) ../../arhdr_libscreen.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libscreen.s
	sh ../../mkar.sh libscreen.a arhdr.o $(OBJS) Library.SC

slibscreen.a: $(OBJS) ../../arhdr_slibscreen.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibscreen.s
	sh ../../mkar.sh slibscreen.a arhdr.o $(OBJS) Library.SC

###################
# obj targets
###################

io.o: ../../io.c
	$(CC) $(CFLAGS) -c ../../io.c
mesg.o: ../../mesg.c
	$(CC) $(CFLAGS) -c ../../mesg.c
mvcursor.o: ../../mvcursor.c
	$(CC) $(CFLAGS) -c ../../mvcursor.c
scrn.o: ../../scrn.c
	$(CC) $(CFLAGS) -c ../../scrn.c

FORCE:

.PHONY: all install clean
