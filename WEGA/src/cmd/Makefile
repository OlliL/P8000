# cmd: WEGA system commands, in the V7 layout: a command with a single source
# file is a file here (ar.c, size.c, ...); a command with several files has
# its own subdirectory with its own Makefile (BOOT0, doscopy, pburn,
# standalone). Start-up variants of the commands live in ../lib/libc/crt.
# The cc driver and popt are Linux-side tools and live in ../../host.
#
#   make                       build everything (with the WEGA cc/cas/ld)
#   make strip-programs        strip the programs that the original ships
#                              stripped (STRIPFILES); run it before comparing
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CAS = cas
CC  = cc
LD  = ld
STRIP = strip
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
CRT = ../lib/libc/crt

PROGS = ar ranlib size strip

FILES=		getud putud dump dumpdir restor sync tar
CLEANFILES=	sync.o ar.o  getud.o  putud.o  ranlib.o  size.o  strip.o
SUBDIRS=	BOOT0 pburn standalone verify copydisk

all: $(PROGS) $(FILES) $(STRIPFILES) subdirs

subdirs:
	-cd doscopy ; $(MAKE) all ; cd ..
	for d in $(SUBDIRS); do cd $$d ; $(MAKE) all ; cd .. ; done

getud.o: getud.c
getud:	getud.o

putud.o: putud.c
putud:	putud.o

dump: dump.c
	$(CC) $(CFLAGS) -s -o $@ dump.c

dumpdir: dumpdir.c
	$(CC) $(CFLAGS) -s -o $@ dumpdir.c

restor: restor.c
	$(CC) $(CFLAGS) -s -o $@ restor.c

tar: tar.c
	$(CC) $(CFLAGS) -s -o $@ tar.c

ar.o: ar.c
	$(CC) $(CFLAGS) -c ar.c

ar: ar.o $(CRT)/crt0-ar.o
	$(LD) -s -e start -o $@ $(CRT)/crt0-ar.o ar.o $(LIBC)

ranlib.o: ranlib.c
	$(CC) $(CFLAGS) -c ranlib.c

ranlib: ranlib.o
	$(LD) -s -e start -o $@ /lib/crt0.o ranlib.o $(LIBC)

size.o: size.c
	$(CC) $(CFLAGS) -c size.c

size: size.o $(CRT)/crt0-size.o
	$(LD) -s -e start -o $@ $(CRT)/crt0-size.o size.o $(LIBC)

strip.o: strip.c
	$(CC) $(CFLAGS) -c strip.c

strip: strip.o $(CRT)/crt0-strip.o
	$(LD) -s -e start -o $@ $(CRT)/crt0-strip.o strip.o $(LIBC)

$(CRT)/crt0-ar.o: $(CRT)/crt0-ar.s
	$(CAS) -u -o $@ $(CRT)/crt0-ar.s

$(CRT)/crt0-size.o: $(CRT)/crt0-size.s
	$(CAS) -u -o $@ $(CRT)/crt0-size.s

$(CRT)/crt0-strip.o: $(CRT)/crt0-strip.s
	$(CAS) -u -o $@ $(CRT)/crt0-strip.s

$(CRT)/crt0-sync.o: $(CRT)/crt0-sync.s
	$(CAS) -u -o $@ $(CRT)/crt0-sync.s

sync.o: sync.c
sync: $(CRT)/crt0-sync.o sync.o
	$(LD) -s -o $@ -e start $(CRT)/crt0-sync.o sync.o -lc

install: $(PROGS) $(FILES) $(STRIPFILES)
	cp ar $(PREFIX)/bin/ar
	cp size $(PREFIX)/bin/size
	cp ranlib $(PREFIX)/bin/ranlib
	cp strip $(PREFIX)/usr/bin/strip
	for d in $(SUBDIRS) doscopy; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	rm -f $(PROGS) $(FILES) $(STRIPFILES) $(CLEANFILES)
	rm -f $(CRT)/crt0-ar.o $(CRT)/crt0-size.o $(CRT)/crt0-strip.o $(CRT)/crt0-sync.o
	for d in $(SUBDIRS) doscopy; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all subdirs clean install strip-programs
