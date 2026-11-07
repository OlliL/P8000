# sld: the segmented loader of WEGA, reconstructed from /bin/sld.
#
#   make link        sld, linked with slibc-v9.a
#   make install PREFIX=dir
#   make clean

SCC = scc
CRT = ../../lib/libc/crt
LDBIN = sld
CAS = cas
LIBC = ../../lib/slibc-v9.a
CFLAGS = -O
PREFIX = /
STRIP = strip

COBJS = copyname.o dump.o ext.o ld.o ldm.o printname.o swap.o symbols.o
LINKOBJS = scrt0.o $(COBJS)

all: sld

link: sld

sld: $(LINKOBJS) $(LIBC)
	$(LDBIN) -i -z -e start $(LINKOBJS) $(LIBC) -o $@

install: sld
	cp sld $(PREFIX)/bin/sld
	$(STRIP) $(PREFIX)/bin/sld

copyname.o: copyname.c ld.h
	$(SCC) $(CFLAGS) -c copyname.c

dump.o: dump.c ld.h
	$(SCC) $(CFLAGS) -c dump.c

ext.o: ext.c ld.h
	$(SCC) $(CFLAGS) -c ext.c

ld.o: ld.c ld.h
	$(SCC) $(CFLAGS) -c ld.c

ldm.o: ldm.c ld.h
	$(SCC) $(CFLAGS) -c ldm.c

printname.o: printname.c ld.h
	$(SCC) $(CFLAGS) -c printname.c

swap.o: swap.c ld.h
	$(SCC) $(CFLAGS) -c swap.c

symbols.o: symbols.c ld.h
	$(SCC) $(CFLAGS) -c symbols.c

scrt0.s: $(CRT)/mkcrt0.sh $(CRT)/scrt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh sld > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

clean:
	rm -f *.o scrt0.s sld
