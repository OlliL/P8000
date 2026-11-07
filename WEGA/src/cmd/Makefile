# cmd: WEGA system commands, in the V7 layout: a command with a single source
# file is a file here (ar.c, mv.c, size.c, ...); a command with several files, several versions or its own
# libc has its own subdirectory with its own Makefile (BOOT0, doscopy, pburn, standalone, dc, cc, cas,
# cpp, cparse, codgen, popt, ld, sld, verify, copydisk, format). Start-up variants of the commands live in
# ../lib/libc/crt (crt0.s.in + crt0.list, made into crt0-<command>.s by mkcrt0.sh).
# cc/cc.c is the source of both drivers, cc and scc (scc is cc.c compiled with -DSCC).
#
#   make                       build everything (with the WEGA cc/scc/cas/as/ld/sld/gld/yacc)
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the programs

CAS = cas
CC  = cc
CCV0 = ccv0
CCV1 = ccv1
CCV1R = ccv1r
LD  = ld
STRIP = strip
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
CRT = ../lib/libc/crt
# the libc of each single-file command (the one of its time, as in its own Makefile before)
LIBC_V1 = ../lib/libc-v1.a
LIBC_V3 = ../lib/libc-v3.a
LIBC_V4 = ../lib/libc-v4.a
LIBC_V6 = ../lib/libc-v6.a
LIBC_V10 = ../lib/libc-v10.a

PROGS = ar

# single-file commands, each with its own compiler and its own libc (byte-identical with the original)
SIMPLE=		mv rm rmdir write size strip ranlib

FILES=		getud putud dump dumpdir restor sync tar
CLEANFILES=	sync.o ar.o  getud.o  putud.o
SUBDIRS=	doscopy BOOT0 pburn standalone verify copydisk format cc cas cpp cparse codgen popt dc ld sld

all: $(PROGS) $(FILES) $(SIMPLE) subdirs

subdirs:
	for d in $(SUBDIRS); do cd $$d ; $(MAKE) all ; cd .. ; done

getud: getud.c
	$(CC) -o $@ getud.c
	@echo "Patching getud with scratchfilename used in original cc (contains PID)"
	@dd if=getud of=getud1 bs=1 count=6975 >/dev/null 2>&1
	@echo -n '524.o' >>getud1
	@dd if=getud of=getud2 bs=1 skip=6980 >/dev/null 2>&1
	@cat getud2 >> getud1
	@mv getud1 getud
	@rm getud2

putud: putud.c
	$(CC) -o $@ putud.c
	@echo "Patching putud with scratchfilename used in original cc (contains PID)"
	@dd if=putud of=putud1 bs=1 count=6885 >/dev/null 2>&1
	@echo -n '330.o' >>putud1
	@dd if=putud of=putud2 bs=1 skip=6890 >/dev/null 2>&1
	@cat putud2 >> putud1
	@mv putud1 putud
	@rm putud2

dump: dump.c
	$(CC) $(CFLAGS) -o $@ dump.c

dumpdir: dumpdir.c
	$(CC) $(CFLAGS) -o $@ dumpdir.c

restor: restor.c
	$(CC) $(CFLAGS) -o $@ restor.c

tar: tar.c
	$(CC) $(CFLAGS) -o $@ tar.c

ar.o: ar.c
	$(CC) $(CFLAGS) -c ar.c

ar: ar.o crt0-ar.o
	$(LD) -e start -o $@ crt0-ar.o ar.o $(LIBC)

crt0-ar.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ar > $@

crt0-ar.o: crt0-ar.s
	$(CAS) -u -o $@ crt0-ar.s

crt0-sync.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh sync > $@

crt0-sync.o: crt0-sync.s
	$(CAS) -u -o $@ crt0-sync.s

sync.o: sync.c
sync: crt0-sync.o sync.o
	$(LD) -o $@ -e start crt0-sync.o sync.o -lc

# The single-file commands: each with its own compiler (as before in their own Makefiles), its own libc
# and its own start-up file crt0-NAME.o. -Icparse: the stdio.h of the time (as ../cparse before).
# mv: also cp and ln (hard links), byte-identical with the originals
mv.o: mv.c
	$(CCV1) $(CFLAGS) -Icparse -c mv.c
mv: crt0-mv.o mv.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-mv.o mv.o $(LIBC_V6)

rm.o: rm.c
	$(CCV1) $(CFLAGS) -Icparse -c rm.c
rm: crt0-rm.o rm.o $(LIBC_V6)
	$(LD) -e start -o $@ crt0-rm.o rm.o $(LIBC_V6)

rmdir.o: rmdir.c
	$(CCV1R) $(CFLAGS) -Icparse -c rmdir.c
rmdir: crt0-rmdir.o rmdir.o $(LIBC_V10)
	$(LD) -e start -o $@ crt0-rmdir.o rmdir.o $(LIBC_V10)

write.o: write.c
	$(CCV1R) $(CFLAGS) -Icparse -c write.c
write: crt0-write.o write.o $(LIBC_V10)
	$(LD) -e start -o $@ crt0-write.o write.o $(LIBC_V10)

size.o: size.c
	$(CCV1) $(CFLAGS) -c size.c
size: crt0-size.o size.o $(LIBC_V3)
	$(LD) -e start -o $@ crt0-size.o size.o $(LIBC_V3)

strip.o: strip.c
	$(CCV1) $(CFLAGS) -c strip.c
strip: crt0-strip.o strip.o $(LIBC_V4)
	$(LD) -e start -o $@ crt0-strip.o strip.o $(LIBC_V4)

ranlib.o: ranlib.c
	$(CCV0) $(CFLAGS) -Icparse -c ranlib.c
ranlib: crt0-ranlib.o ranlib.o $(LIBC_V1)
	$(LD) -e start -o $@ crt0-ranlib.o ranlib.o $(LIBC_V1)

crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh `echo $@ | sed -e 's/^crt0-//' -e 's/\.s$$//'` > $@

crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o: crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s
	$(CAS) -u -o $@ `echo $@ | sed -e 's/\.o$$/.s/'`

install: $(PROGS) $(FILES) $(SIMPLE)
	for file in ar chkdiff getud putud tar uuclear whodo sync ; do cp $$file $(PREFIX)/bin/$$file ; done
	for file in ar tar sync ; do $(STRIP) $(PREFIX)/bin/$$file ; done
	for file in dump dumpdir restor ; do cp $$file $(PREFIX)/etc/$$file ; $(STRIP) $(PREFIX)/etc/$$file ; done
	cp rm $(PREFIX)/bin/rm ; $(STRIP) $(PREFIX)/bin/rm
	cp rmdir $(PREFIX)/bin/rmdir ; $(STRIP) $(PREFIX)/bin/rmdir
	cp write $(PREFIX)/bin/write ; $(STRIP) $(PREFIX)/bin/write
	cp size $(PREFIX)/bin/size ; $(STRIP) $(PREFIX)/bin/size
	cp strip $(PREFIX)/usr/bin/strip ; $(STRIP) $(PREFIX)/usr/bin/strip
	cp ranlib $(PREFIX)/usr/bin/ranlib ; $(STRIP) $(PREFIX)/usr/bin/ranlib
	cp mv $(PREFIX)/bin/mv ; $(STRIP) $(PREFIX)/bin/mv
	rm -f $(PREFIX)/bin/cp $(PREFIX)/bin/ln
	ln $(PREFIX)/bin/mv $(PREFIX)/bin/cp
	ln $(PREFIX)/bin/mv $(PREFIX)/bin/ln
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	rm -f $(PROGS) $(FILES) $(CLEANFILES) $(SIMPLE)
	rm -f mv.o rm.o rmdir.o write.o size.o strip.o ranlib.o
	rm -f crt0-ar.o crt0-sync.o crt0-ar.s crt0-sync.s
	rm -f crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o
	rm -f crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all subdirs clean install
