# sld: the segmented loader of WEGA, reconstructed from /bin/sld.
#
#   make link        sld, linked with slibc-v9.a
#   make old         sldo, an old sld (ldm.c with -DOLD_SLDS: a stack segment of -Ms without contents gets no table
#                    entry; the header of /lib/gopt); make oldinstall PREFIX=dir puts it to dir/bin/sldo
#   make install PREFIX=dir
#   make clean

SCC = scc
CRT = ../../lib/libc/nonseg/crt
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

old: sldo

sldo: scrt0.o copyname.o dump.o ext.o ld.o ldmo.o printname.o swap.o symbols.o $(LIBC)
	$(LDBIN) -i -z -e start scrt0.o copyname.o dump.o ext.o ld.o ldmo.o printname.o swap.o symbols.o $(LIBC) -o $@

ldmo.o: ldm.c ld.h
	cp ldm.c ldmo.c
	$(SCC) $(CFLAGS) -DOLD_SLDS -c ldmo.c
	rm -f ldmo.c

oldinstall: sldo
	cp sldo $(PREFIX)/bin/sldo

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
	rm -f *.o scrt0.s sld sldo

# the library of /lib is not made here (make shipped)
/lib/libc.a /lib/slibc.a:
