#
# makefile for lcopt
#

SHELL=/bin/sh

OWN=	wega
GRP=	system
PROT=	751

ROOT=	/z/LCC
LIB=	$(ROOT)/lib
CC=	lcc

CFLAGS=	-O

all:  lcopt

lcopt:	c20.o c21.o
	lcc -o lcopt c2*.o
	chmog $(PROT) $(OWN) $(GRP) lcopt

c20.o c21.o:	c2.h

move:
	rm -f $(LIB)/lcopt
	cp lcopt $(LIB)/lcopt
	chmog $(PROT) $(OWN) $(GRP) $(LIB)/lcopt

install: all save move

#--------------------------

uninstall:	$(LIB)/OLDlcopt
		
		mv $(LIB)/OLDlcopt $(LIB)/lcopt

#--------------------------

save:		
	if test -f $(LIB)/lcopt ; \
	then \
		rm -f $(LIB)/OLDlcopt ; \
		mv $(LIB)/lcopt $(LIB)/OLDlcopt ; \
	fi

clean:
	-rm -f *.o lcopt

clobber:
	-rm -f *.o lcopt

c.o. :
	cc $(CFLAGS) -c $<
