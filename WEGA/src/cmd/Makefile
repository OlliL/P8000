# cmd: WEGA system commands, in the V7 layout: a command with a single source
# file is a file here (ar.c, tar.c, ...); a command with several files, several versions or its own
# libc has its own subdirectory with its own Makefile (BOOT0, doscopy, pburn, standalone, size, strip,
# ranlib, cc, cas, codgen). Start-up variants of the commands live in ../lib/libc/crt (crt0.s.in + crt0.list,
# made into crt0-<command>.s by mkcrt0.sh).
# cc/cc.c is the source of both drivers, cc and scc (scc is cc.c compiled with -DSCC).
# The Linux versions of the cc driver (with cc.conf) and of popt live in ../../host.
#
#   make                       build everything (with the WEGA cc/scc/cas/as/ld/sld/gld/yacc)
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

PROGS = ar

FILES=		getud putud dump dumpdir restor sync tar
CLEANFILES=	sync.o ar.o  getud.o  putud.o
SUBDIRS=	BOOT0 pburn standalone verify copydisk format size strip ranlib cc cas codgen

all: $(PROGS) $(FILES) subdirs

subdirs:
	-cd doscopy ; $(MAKE) all ; cd ..
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

$(CRT)/crt0-ar.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ar > $@

$(CRT)/crt0-ar.o: $(CRT)/crt0-ar.s
	$(CAS) -u -o $@ $(CRT)/crt0-ar.s

$(CRT)/crt0-sync.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh sync > $@

$(CRT)/crt0-sync.o: $(CRT)/crt0-sync.s
	$(CAS) -u -o $@ $(CRT)/crt0-sync.s

sync.o: sync.c
sync: $(CRT)/crt0-sync.o sync.o
	$(LD) -s -o $@ -e start $(CRT)/crt0-sync.o sync.o -lc

install: $(PROGS) $(FILES)
	for file in ar chkdiff getud putud tar uuclear whodo sync ; do cp $$file $(PREFIX)/bin/$$file ; done
	for file in dump dumpdir restor ; do cp $$file $(PREFIX)/etc/$$file ; done
	for d in $(SUBDIRS) doscopy; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	rm -f $(PROGS) $(FILES) $(CLEANFILES)
	rm -f $(CRT)/crt0-ar.o $(CRT)/crt0-sync.o $(CRT)/crt0-ar.s $(CRT)/crt0-sync.s
	for d in $(SUBDIRS) doscopy; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all subdirs clean install
