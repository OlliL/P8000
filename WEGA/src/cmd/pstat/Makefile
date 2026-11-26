# pstat: /bin/pstat (segmented), pstat.c 3.2 11/18/83 (no source known: rebuilt from the binary on the frame of V7's
# pstat.c; the table sizes from the kernel (_Ninode ...), calloc, the sio lines, -u with the WEGA user structure, a crash
# dump through kva.c and kpa.c of ps), by the shipped scc; kva.c and kpa.c (../ps) by sccvk as for ps. The shipped
# slibc.a. The start-up file from crt0.list (scrt0, _environ in segment 0) with _environ as a common of its own, as ps.
#
#   make              build ./pstat
#   make install PREFIX=dir
#   make clean

SCC = sccvk
SCCVK = sccvk
CAS = cas
SLD = sld
CFLAGS = -O -I../../head
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc/build/seg/slibc.a
CRT = ../../lib/libc/nonseg/crt

all: pstat

pstat: scrt0.o pstat.o kva.o kpa.o $(LIBC)
	$(SLD) -z -e start -o $@ scrt0.o pstat.o kva.o kpa.o $(LIBC)

scrt0.s: $(CRT)/scrt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh pstat | sed -e 's/^	ldl	<<0>>%0000,rr2$$/	ldl	_environ,rr2/' -e 's/^	.extern	__callex$$/	.extern	__callex\
	.comm	4,_environ/' > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

pstat.o: pstat.c
	$(SCC) -I../../head -c pstat.c

kva.o: ../ps/kva.c
	cp ../ps/kva.c kva.c ; $(SCCVK) $(CFLAGS) -c kva.c ; rm -f kva.c

kpa.o: ../ps/kpa.c
	cp ../ps/kpa.c kpa.c ; $(SCCVK) $(CFLAGS) -c kpa.c ; rm -f kpa.c

install: pstat
	cp pstat $(PREFIX)/bin/pstat
	$(STRIP) $(PREFIX)/bin/pstat

clean:
	rm -f *.o scrt0.s pstat
