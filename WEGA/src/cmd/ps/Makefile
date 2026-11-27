# ps: /bin/ps (segmented), ps.c 3.2 11/18/83: Plexus' ps.c (System III) as WEGA changed it (/wega with _proc, _swplo,
# _Nproc; -k kcore for a crash dump; the arguments through the user stack), by the shipped scc; kva.c and kpa.c
# (08/25/83, a crash dump: the kernel and the physical addresses) by sccvk, the compiler of the kernel objects
# (host/OLDCC.md, vk). The shipped slibc.a. The start-up file from crt0.list (scrt0, _environ in segment 0) with
# _environ as a common of its own: ps is linked in one segment (sld -z) and _environ is the first word of its bss.
#
#   make              build ./ps
#   make install PREFIX=dir
#   make clean

SCC = scc
SCCVK = sccvk
CAS = cas
SLD = sld
CFLAGS = -O -I../../head
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc/build/seg/slibc.a
CRT = ../../lib/libc/nonseg/crt

all: ps

ps: scrt0.o ps.o kva.o kpa.o $(LIBC)
	$(SLD) -z -e start -o $@ scrt0.o ps.o kva.o kpa.o $(LIBC)

scrt0.s: $(CRT)/scrt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ps | sed -e 's/^	ldl	<<0>>%0000,rr2$$/	ldl	_environ,rr2/' | awk '{ print } /^	.extern	__callex$$/ { print "	.comm	4,_environ" }' > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

ps.o: ps.c
	$(SCC) $(CFLAGS) -c ps.c

kva.o: kva.c
	$(SCCVK) $(CFLAGS) -c kva.c

kpa.o: kpa.c
	$(SCCVK) $(CFLAGS) -c kpa.c

install: ps
	cp ps $(PREFIX)/bin/ps
	$(STRIP) $(PREFIX)/bin/ps

clean:
	rm -f *.o scrt0.s ps

# the library of /lib is not made here (the shipped build)
/lib/slibc.a:
