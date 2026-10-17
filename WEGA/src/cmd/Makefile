# cmd: WEGA system commands, in the V7 layout: a command with a single source
# file is a file here (ar.c, size.c, ...); a command with several files has
# its own subdirectory with its own Makefile (doscopy, pburn, standalone,
# BOOT0). Start-up variants of the commands live in ../lib/libc/crt.
# The cc driver and popt are Linux-side tools and live in ../../host.
#
#   make                       build everything (with the WEGA cc/cas/ld)
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CAS ?= cas
CC  ?= cc
LD  ?= ld
STRIP ?= strip
CFLAGS ?= -O
PREFIX ?= /
LIBC ?= /lib/libc.a
CRT = ../lib/libc/crt

PROGS = ar ranlib size strip

FILES=		getud putud
STRIPFILES=	copydisk dump dumpdir format restor sync tar verify
CLEANFILES=	sync.o

all: $(PROGS) $(FILES) $(STRIPFILES) strip-programs

ar: ar.o $(CRT)/crt0-ar.o
	$(LD) -s -e start -o $@ $(CRT)/crt0-ar.o ar.o $(LIBC)

size: size.o $(CRT)/crt0-size.o
	$(LD) -s -e start -o $@ $(CRT)/crt0-size.o size.o $(LIBC)

strip: strip.o $(CRT)/crt0-strip.o
	$(LD) -s -e start -o $@ $(CRT)/crt0-strip.o strip.o $(LIBC)

ranlib: ranlib.o
	$(LD) -s -e start -o $@ /lib/crt0.o ranlib.o $(LIBC)

$(CRT)/crt0-ar.o: $(CRT)/crt0-ar.s
	$(CAS) -u -o $@ $(CRT)/crt0-ar.s

$(CRT)/crt0-size.o: $(CRT)/crt0-size.s
	$(CAS) -u -o $@ $(CRT)/crt0-size.s

$(CRT)/crt0-strip.o: $(CRT)/crt0-strip.s
	$(CAS) -u -o $@ $(CRT)/crt0-strip.s

strip-programs:
	$(STRIP) $(STRIPFILES)

$(CRT)/crt0-sync.o: $(CRT)/crt0-sync.s
	$(CAS) -u -o $@ $(CRT)/crt0-sync.s

sync: $(CRT)/crt0-sync.o sync.o
	$(LD) -o $@ -e start $(CRT)/crt0-sync.o sync.o -lc

install: $(PROGS)
	cp ar $(PREFIX)/bin/ar
	cp size $(PREFIX)/bin/size
	cp ranlib $(PREFIX)/bin/ranlib
	cp strip $(PREFIX)/usr/bin/strip

clean:
	rm -f $(PROGS) $(FILES) $(STRIPFILES) $(CLEANFILES) *.o
	rm -f $(CRT)/crt0-ar.o $(CRT)/crt0-size.o $(CRT)/crt0-strip.o $(CRT)/crt0-sync.o

.PHONY: all clean install strip-programs
