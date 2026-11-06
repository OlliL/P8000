# cparse, scparse: pass 1 of cc and scc (PCC front end) of WEGA, reconstructed from /lib/cparse. scparse is the same source with -DSEG,
# the objects in seg. With strip byte-identical.
#
#   make	     cparse
#   make scparse
#   make oldprep     what the versions of the old compiler need of this directory (crt0.o, cgram1.c)
#   make old	 the versions of the old compiler in OLDDIRS (v1/Makefile ...)
#   make clean
#
# The parser cgram1.y is made into cgram1.c by yacc.

CC = ccv0p
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O
CRT = ../../lib/libc/crt
LIBC = ../../lib/libc-v7.a
OBJS = cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o sym2.o
SOBJS = seg/cgram1.o seg/xdefs1.o seg/scan1.o seg/pftn1.o seg/trees1.o seg/optim1.o seg/code1.o seg/local1.o seg/comm1.o seg/vinit1.o seg/sym1.o seg/sym2.o
HDRS = mfile1 manifest macdefs stdio.h
OLDDIRS = v1 v0 v0p

CC = ccv0p
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O
PREFIX = /
LIBC = ../../../../lib/libc-v7.a
CRT = ../../lib/libc/crt

NONSEG_OBJS = cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o sym2.o
SEG_OBJS = $(NONSEG_OBJS)
ALL_TARGETS = build/nonseg/cparse build/seg/scparse
OLD_TARGETS = build/nonsegv0/cparse build/nonsegv0p/cparse build/nonsegv1/cparse build/segv1/scparse

all: $(ALL_TARGETS)

old: $(OLD_TARGETS)

build/nonseg/cparse: crt0.o
	cd build/nonseg    && make -f ../../Makefile cparse
build/nonsegv0/cparse: crt0.o
	cd build/nonsegv0  && make -f ../../Makefile cparse CC="cc" CFLAGS="$(CFLAGS) -DOLD_D12 -DOLD_D4R -DOLD_D6T -DOLD_D6C -DOLD_D22 -DOLD_D6D"
build/nonsegv0p/cparse: crt0.o
	cd build/nonsegv0p && make -f ../../Makefile cparse CC="cc" CFLAGS="$(CFLAGS) -DOLD_D12 -DOLD_D4R -DOLD_D6T -DOLD_D6C -DOLD_D22 -DOLD_D6DP"
build/nonsegv1/cparse: crt0.o
	cd build/nonsegv1  && make -f ../../Makefile cparse CC="cc" CFLAGS="$(CFLAGS) -DOLD_D12 -DOLD_D1"

build/seg/scparse: crt0.o
	cd build/seg   && make -f ../../Makefile scparse CFLAGS="$(CFLAGS) -DSEG"
build/segv1/scparse: crt0.o
	cd build/segv1 && make -f ../../Makefile scparse CC="cc" CFLAGS="$(CFLAGS) -DSEG -DOLD_D12 -DOLD_D1"

clean:
	rm -f build/nonseg/*
	rm -f build/nonsegv0/*
	rm -f build/nonsegv0p/*
	rm -f build/nonsegv1/*
	rm -f build/seg/*
	rm -f build/segv1/*
	rm -f crt0.o crt0.s

install: $(ALL_TARGETS)
	cp build/nonseg/cparse $(PREFIX)/lib/cparse
	cp build/seg/scparse   $(PREFIX)/lib/scparse

oldinstall: $(OLD_TARGETS)
	for v in v0 v0p v1 ; do \
		[ -d $(PREFIX)/lib/$$v ] || mkdir $(PREFIX)/lib/$$v ; \
		cp build/nonseg$$v/cparse $(PREFIX)/lib/$$v/cparse ; \
	done
	cp build/segv1/scparse $(PREFIX)/lib/v1/scparse

#################
# CRT0 Target
##################
crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh cparse > $@

crt0.o: crt0.s
	$(CAS) -u crt0.s

##################
# bin targets
##################

cparse: $(NONSEG_OBJS) ../../crt0.o
	$(LD) -e start -i -o $@ ../../crt0.o $(NONSEG_OBJS) $(LIBC)

scparse: $(SEG_OBJS) ../../crt0.o
	$(LD) -e start -i -o $@ ../../crt0.o $(SEG_OBJS)    $(LIBC)

###################
# obj targets
###################

cgram1.o: ../../cgram1.c
	$(CC) $(CFLAGS) -c ../../cgram1.c
xdefs1.o: ../../xdefs1.c
	$(CC) $(CFLAGS) -c ../../xdefs1.c
scan1.o: ../../scan1.c
	$(CC) $(CFLAGS) -c ../../scan1.c
pftn1.o: ../../pftn1.c
	$(CC) $(CFLAGS) -c ../../pftn1.c
trees1.o: ../../trees1.c
	$(CC) $(CFLAGS) -c ../../trees1.c
optim1.o: ../../optim1.c
	$(CC) $(CFLAGS) -c ../../optim1.c
code1.o: ../../code1.c
	$(CC) $(CFLAGS) -c ../../code1.c
local1.o: ../../local1.c
	$(CC) $(CFLAGS) -c ../../local1.c
comm1.o: ../../comm1.c
	$(CC) $(CFLAGS) -c ../../comm1.c
vinit1.o: ../../vinit1.c
	$(CC) $(CFLAGS) -c ../../vinit1.c
sym1.o: ../../sym1.c
	$(CC) $(CFLAGS) -c ../../sym1.c
sym2.o: ../../sym2.c
	$(CC) $(CFLAGS) -c ../../sym2.c

.PHONY: all clean old oldinstall
