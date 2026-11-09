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

# shipped: everything built with the shipped cc and the shipped libc (CC=cc, /lib/libc.a, /lib/slibc.a) and -DSHIPPED, which
# switches the sources to what the shipped libc and cc need (the old libc's readsym, sprintf value, swap_quad; the exit status
# of size). Without -DSHIPPED the bytes of the originals are rebuilt. The files of /lib are not made here: empty rules below.
SHIPFLAGS = -O -DSHIPPED
SHIPCC = CC=cc CCV0=cc CCV1=cc CCV1R=cc CFLAGS="$(SHIPFLAGS)" LIBC_V1=/lib/libc.a LIBC_V3=/lib/libc.a LIBC_V4=/lib/libc.a LIBC_V6=/lib/libc.a LIBC_V10=/lib/libc.a

shipped:
	$(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) $(PROGS) $(FILES) $(SIMPLE) $(SHIPCC) LIBC=/lib/libc.a
	cd ../lib/oldlibc/printf ; cp 0_5.c s1.c ; cc -O -c s1.c ; rm -f s1.c
	cd ../lib/oldlibc/doprnt ; $(CAS) -u -o 0_5.o 0_5.s
	for d in $(SUBDIRS) ; do \
		case $$d in \
		sld) X="SCC=scc LIBC=/lib/slibc.a" ;; \
		cas) X="SCC=scc" ;; \
		codgen|popt) X='LIBC="../../../../lib/oldlibc/printf/s1.o ../../../../lib/oldlibc/doprnt/0_5.o /lib/libc.a"' ;; \
		cpp|cparse|cc|dc|ld) X="LIBC=/lib/libc.a" ;; \
		*) X="" ;; \
		esac ; \
		cd $$d ; eval $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all '$(SHIPCC)' $$X ; cd .. ; \
	done

/lib/libc.a /lib/slibc.a:

subdirs:
	for d in $(SUBDIRS); do cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all ; cd .. ; done

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
	for file in ar getud putud tar sync ; do cp $$file $(PREFIX)/bin/$$file ; done
	for file in chkdiff uuclear whodo ; do cp $$file.sh $(PREFIX)/bin/$$file ; chmod 755 $(PREFIX)/bin/$$file ; done
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
	# the scripts of /etc (NAME.sh, as in System III), with the modes of the originals; mfs=umfs and makelost+found=mlf are links
	cp adduser.sh $(PREFIX)/etc/adduser ; chmod 700 $(PREFIX)/etc/adduser
	cp down.sh $(PREFIX)/etc/down ; chmod 700 $(PREFIX)/etc/down
	cp halt.sh $(PREFIX)/etc/halt ; chmod 700 $(PREFIX)/etc/halt
	cp install.sh $(PREFIX)/etc/install ; chmod 750 $(PREFIX)/etc/install
	cp killall.sh $(PREFIX)/etc/killall ; chmod 744 $(PREFIX)/etc/killall
	cp makelost+found.sh $(PREFIX)/etc/makelost+found ; chmod 700 $(PREFIX)/etc/makelost+found
	cp makewhatis.sh $(PREFIX)/etc/makewhatis ; chmod 700 $(PREFIX)/etc/makewhatis
	cp man.dbmgr.sh $(PREFIX)/etc/man.dbmgr ; chmod 700 $(PREFIX)/etc/man.dbmgr
	cp mfs.sh $(PREFIX)/etc/mfs ; chmod 750 $(PREFIX)/etc/mfs
	cp mvdir.sh $(PREFIX)/etc/mvdir ; chmod 700 $(PREFIX)/etc/mvdir
	cp new.install.sh $(PREFIX)/etc/new.install ; chmod 700 $(PREFIX)/etc/new.install
	cp rmuser.sh $(PREFIX)/etc/rmuser ; chmod 700 $(PREFIX)/etc/rmuser
	cp sysgen.sh $(PREFIX)/etc/sysgen ; chmod 751 $(PREFIX)/etc/sysgen
	cp UGETT1.sh $(PREFIX)/etc/UGETT1 ; chmod 700 $(PREFIX)/etc/UGETT1
	rm -f $(PREFIX)/etc/umfs $(PREFIX)/etc/mlf
	ln $(PREFIX)/etc/mfs $(PREFIX)/etc/umfs
	ln $(PREFIX)/etc/makelost+found $(PREFIX)/etc/mlf
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	rm -f $(PROGS) $(FILES) $(CLEANFILES) $(SIMPLE)
	rm -f mv.o rm.o rmdir.o write.o size.o strip.o ranlib.o
	rm -f crt0-ar.o crt0-sync.o crt0-ar.s crt0-sync.s
	rm -f crt0-mv.o crt0-rm.o crt0-rmdir.o crt0-write.o crt0-size.o crt0-strip.o crt0-ranlib.o
	rm -f crt0-mv.s crt0-rm.s crt0-rmdir.s crt0-write.s crt0-size.s crt0-strip.s crt0-ranlib.s
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) clean ; cd .. ; done

.PHONY: all subdirs clean install
