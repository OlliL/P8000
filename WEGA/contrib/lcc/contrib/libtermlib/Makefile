#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1 (Berkeley) 6/5/85
#
_LIB=		libtermlib.a

LIB=		L$(_LIB)
PREFIX=		/z/LCC
CC=		lcc
LD=		ldd

#LIB=		$(_LIB)
#PREFIX=	/usr
#CC=		cc
#LD=		ld

#LIB=		s$(_LIB)
#PREFIX=	/usr
#CC=		scc
#LD=		sld

CFLAGS=		-O -DCM_N -DCM_GT -DCM_B -DCM_D

AR=		-ar
ARFLAGS=	rvu

CHMOG=		chmog
CP=		cp
RANLIB=		ranlib
LIBMODE=	644
LIBOWNER=	wega
LIBGROUP=	system

SRCS=	termcap.c tgoto.c tputs.c
OBJS=	termcap.o tgoto.o tputs.o

all:	$(LIB)

$(LIB): ${OBJS}
	ar cr $(LIB) $(OBJS)

install:	all
	$(CP) $(LIB) $(PREFIX)/lib
	$(CHMOG) $(LIBMODE) $(LIBOWNER) $(LIBGROUP) $(PREFIX)/lib/$(LIB)
	$(RANLIB) $(PREFIX)/lib/$(LIB)

clean:
	-rm -f *.o $(LIB)
