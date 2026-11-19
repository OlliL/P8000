# libmp: /usr/lib/libmp.a and /usr/lib/slibmp.a (multiple precision integers, nonsegmented and segmented). System III
# has no source: from 2.11BSD (usr.lib/libmp; its sccsid lines out). Both libraries are made from the same sources
# (Library.SC 02/11/83) by v1 (ccv1, sccv1 from OLDROOT) with the stdio.h of V7 (src/head/oldv7: mp.h includes it).
# WEGA's forms: compare/libmp/SYSIII.md. Headers: arhdr_libmp.s, arhdr_slibmp.s, mkar.sh.
#
#   make        libmp.a slibmp.a
#   make install PREFIX=dir   dir/usr/lib/libmp.a, dir/usr/lib/slibmp.a
#   make clean

CC = ccv1
SCC = sccv1
CAS = cas
PREFIX = /
CFLAGS = -O -I../../../../head/oldv7
OBJS = pow.o gcd.o msqrt.o mdiv.o mout.o mult.o madd.o util.o

all: build/nonseg/libmp.a build/seg/slibmp.a

build/nonseg/libmp.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile libmp.a

build/seg/slibmp.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) -f ../../Makefile slibmp.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libmp.a $(PREFIX)/usr/lib/libmp.a
	cp build/seg/slibmp.a $(PREFIX)/usr/lib/slibmp.a

clean:
	cd build/nonseg ; rm -f libmp.a  arhdr.o $(OBJS) Library.SC
	cd build/seg    ; rm -f slibmp.a arhdr.o $(OBJS) Library.SC

##################
# bin targets
##################

libmp.a: $(OBJS) ../../arhdr_libmp.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libmp.s
	sh ../../mkar.sh libmp.a arhdr.o $(OBJS) Library.SC

slibmp.a: $(OBJS) ../../arhdr_slibmp.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibmp.s
	sh ../../mkar.sh slibmp.a arhdr.o $(OBJS) Library.SC

###################
# obj targets
###################

pow.o: ../../pow.c
	$(CC) $(CFLAGS) -c ../../pow.c
gcd.o: ../../gcd.c
	$(CC) $(CFLAGS) -c ../../gcd.c
msqrt.o: ../../msqrt.c
	$(CC) $(CFLAGS) -c ../../msqrt.c
mdiv.o: ../../mdiv.c
	$(CC) $(CFLAGS) -c ../../mdiv.c
mout.o: ../../mout.c
	$(CC) $(CFLAGS) -c ../../mout.c
mult.o: ../../mult.c
	$(CC) $(CFLAGS) -c ../../mult.c
madd.o: ../../madd.c
	$(CC) $(CFLAGS) -c ../../madd.c
util.o: ../../util.c
	$(CC) $(CFLAGS) -c ../../util.c

FORCE:

.PHONY: all install clean
