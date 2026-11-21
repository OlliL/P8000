# libcisam: /usr/lib/libcisam.a and /usr/lib/slibcisam.a (C-ISAM 1.03 of Informix, nonsegmented and segmented). No source is
# known: the 14 sources and isint.h are written from the original members (compare/libcisam/COMPILER.md, compare/libcisam/findings.json).
# Both libraries are made from the same sources (Library.SC 02/11/83) by v1c (ccv1c, sccv1c from OLDROOT: v1 with the code
# generator of libcisam, host/OLDCC.md) with -O; their Library.SC differ in the times of the check-out:
# Library_libcisam.SC, Library_slibcisam.SC. Headers: arhdr_libcisam.s, arhdr_slibcisam.s, mkar.sh.
#
#   make        libcisam.a slibcisam.a
#   make install PREFIX=dir   dir/usr/lib/libcisam.a, dir/usr/lib/slibcisam.a
#   make clean

CC = ccv1c
SCC = sccv1c
CAS = cas
PREFIX = /
CFLAGS = -O -I../..
OBJS = build.o open.o read.o audit.o btree.o node.o item.o key.o lock.o supp.o buf.o util.o isdef.o isperror.o

all: build/nonseg/libcisam.a build/seg/slibcisam.a

build/nonseg/libcisam.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile libcisam.a

build/seg/slibcisam.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) -f ../../Makefile slibcisam.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libcisam.a $(PREFIX)/usr/lib/libcisam.a
	cp build/seg/slibcisam.a $(PREFIX)/usr/lib/slibcisam.a

clean:
	cd build/nonseg ; rm -f libcisam.a  arhdr.o $(OBJS) Library.SC
	cd build/seg    ; rm -f slibcisam.a arhdr.o $(OBJS) Library.SC

##################
# bin targets
##################

libcisam.a: $(OBJS) ../../arhdr_libcisam.s ../../mkar.sh ../../Library_libcisam.SC
	cp ../../Library_libcisam.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libcisam.s
	sh ../../mkar.sh libcisam.a arhdr.o $(OBJS) Library.SC

slibcisam.a: $(OBJS) ../../arhdr_slibcisam.s ../../mkar.sh ../../Library_slibcisam.SC
	cp ../../Library_slibcisam.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibcisam.s
	sh ../../mkar.sh slibcisam.a arhdr.o $(OBJS) Library.SC

###################
# obj targets
###################

build.o: ../../build.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../build.c
open.o: ../../open.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../open.c
read.o: ../../read.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../read.c
audit.o: ../../audit.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../audit.c
btree.o: ../../btree.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../btree.c
node.o: ../../node.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../node.c
item.o: ../../item.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../item.c
key.o: ../../key.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../key.c
lock.o: ../../lock.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../lock.c
supp.o: ../../supp.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../supp.c
buf.o: ../../buf.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../buf.c
util.o: ../../util.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../util.c
isdef.o: ../../isdef.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../isdef.c
isperror.o: ../../isperror.c ../../isint.h
	$(CC) $(CFLAGS) -c ../../isperror.c

FORCE:

.PHONY: all install clean
