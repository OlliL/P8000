# ld: the segmented loader of WEGA (sld), reconstructed from /bin/sld.
#
#   make link        sld, linked with slibc-v9.a
#   make check       the test objects of host/ld/test linked by sld and by the original under z8k-emu, compared (host/ld/check.py -S)
#   make clean

SCC = scc
CRT = ../../lib/libc/crt
LDBIN = sld
CAS = cas
LIBC = ../../lib/slibc-v9.a
CFLAGS = -O
CHECKROOT = ../../../tmp/wega31_root

COBJS = copyname.o dump.o ext.o ld.o ldm.o printname.o swap.o symbols.o
LINKOBJS = scrt0.o $(COBJS)

all: sld

link: sld

sld: $(LINKOBJS) $(LIBC)
	$(LDBIN) -i -z -e start $(LINKOBJS) $(LIBC) -o $@

check: sld
	cd ../../../host/ld ; SLD_NEW=../../src/cmd/sld/sld WEGA=$(CHECKROOT) python3 check.py -S

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
