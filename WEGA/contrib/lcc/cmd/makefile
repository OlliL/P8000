#
# makefile for commands
#


SHELL=	/bin/sh
INSTDIR= /z/LCC/bin
CC=	lcc
CFLAGS=	-O
PROT=	751
GRP=	system
OWN=	wega


UTILS=\
lcc

all:	ar ccom lcc lcopt lcpp lld


ar:	ar.c
	$(CC) $(CFLAGS) -o ar ar.c
	chmog $(PROT) $(OWN) $(GRP) ar

ccom:
	cd comp/u8k ; make

lcpp:
	cd cpp ; make

lcopt:
	cd c2 ; make

lcc:	lcc.c
	cc -O -i -o lcc lcc.c
	chmog $(PROT) $(OWN) $(GRP) lcc

lld:	lld.c
	$(CC) -o lld lld.c
	chmog $(PROT) $(OWN) $(GRP) lld

install: ar lcc lld
	cd comp/u8k; make install
	cd cpp; make install
	cd c2; make install
	cp ar $(INSTDIR)
	cp lcc $(INSTDIR)
	cp lld $(INSTDIR)
	chmog $(PROT) $(OWN) $(GRP) $(INSTDIR)/ar
	chmog $(PROT) $(OWN) $(GRP) $(INSTDIR)/lcc $(INSTDIR)/lld

