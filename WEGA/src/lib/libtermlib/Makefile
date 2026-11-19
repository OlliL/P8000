# libtermlib: /usr/lib/libtermlib.a and /usr/lib/slibtermlib.a (the termcap library, nonsegmented and segmented). System
# III has no source: from 2.11BSD (usr.lib/libtermlib; its sccsid lines out) with the CM_ options of its Makefile. Both
# libraries are made from the same sources (Library.SC 01/23/83) by v1 (ccv1, sccv1 from OLDROOT). WEGA's (older)
# forms: compare/libtermlib/SYSIII.md. Headers: arhdr_libtermlib.s, arhdr_slibtermlib.s, mkar.sh.
#
#   make        libtermlib.a slibtermlib.a
#   make install PREFIX=dir   dir/usr/lib/libtermlib.a, dir/usr/lib/slibtermlib.a
#   make clean

CC = ccv1
SCC = sccv1
CAS = cas
PREFIX = /
CFLAGS = -O -DCM_N -DCM_GT -DCM_B -DCM_D
OBJS = termcap.o tgoto.o tputs.o

all: build/nonseg/libtermlib.a build/seg/slibtermlib.a

build/nonseg/libtermlib.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile libtermlib.a

build/seg/slibtermlib.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) -f ../../Makefile slibtermlib.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libtermlib.a $(PREFIX)/usr/lib/libtermlib.a
	cp build/seg/slibtermlib.a $(PREFIX)/usr/lib/slibtermlib.a

clean:
	cd build/nonseg ; rm -f libtermlib.a  arhdr.o $(OBJS) Library.SC
	cd build/seg    ; rm -f slibtermlib.a arhdr.o $(OBJS) Library.SC

##################
# bin targets
##################

libtermlib.a: $(OBJS) ../../arhdr_libtermlib.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libtermlib.s
	sh ../../mkar.sh libtermlib.a arhdr.o $(OBJS) Library.SC

slibtermlib.a: $(OBJS) ../../arhdr_slibtermlib.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibtermlib.s
	sh ../../mkar.sh slibtermlib.a arhdr.o $(OBJS) Library.SC

###################
# obj targets
###################

termcap.o: ../../termcap.c
	$(CC) $(CFLAGS) -c ../../termcap.c
tgoto.o: ../../tgoto.c
	$(CC) $(CFLAGS) -c ../../tgoto.c
tputs.o: ../../tputs.c
	$(CC) $(CFLAGS) -c ../../tputs.c

FORCE:

.PHONY: all install clean
