#
# makefile for Llibc
#

OWN=	wega
GRP=	system
PROT=	644

CFLAGS=-O
SHELL=/bin/sh
CC=lcc
AR=ar
LORDER=lorder
ROOT=	/z/LCC
LIB=$(ROOT)/lib

DONE=

all:
	cd port; make
	cd u8k; make all
	-rm -rf object
	mkdir object
	cp port/*/*.o object
	cp u8k/*/*.o object
	-rm -f lib.Llibc
	cd object; mv lcrt0.o ..
	cd object; make -f ../u8k/makefile archive \
		AR=$(AR) LORDER=$(LORDER)
	-rm -rf object
	$(DONE)

move:
	rm -f $(LIB)/lcrt0.o
	cp lcrt0.o $(LIB)/lcrt0.o
	rm -f lcrt0.o
	rm -f $(LIB)/Llibc.a
	cp lib.Llibc $(LIB)/Llibc.a
	chmog $(PROT) $(OWN) $(GRP) $(LIB)/Llibc.a $(LIB)/lcrt0.o

install: all save move

#--------------------------

uninstall:	$(LIB)/OLDLlibc.a \
		$(LIB)/OLDlcrt0.o
		
		mv -f $(LIB)/OLDLlibc.a $(LIB)/Llibc.a
		mv -f $(LIB)/OLDlcrt0.o $(LIB)/lcrt0.o

#--------------------------

save:		
	if test -f $(LIB)/Llibc.a ; \
	then \
		mv $(LIB)/Llibc.a $(LIB)/OLDLlibc.a ; \
	fi

	if test -f $(LIB)/lcrt0.o ; \
	then \
		mv $(LIB)/lcrt0.o $(LIB)/OLDlcrt0.o ; \
	fi ;

clean:
	-rm -rf Llib.contents obj*
	cd port ;  make clean
	cd u8k; make clean

cleanc:
	-rm -rf lib.Llibc Llib.contents obj*
	cd port ;  make clean
	cd u8k; make cleanc

clobber:
	-rm -rf *.o lib.Llibc Llib.contents obj*
	cd port ;  make clobber
	cd u8k; make clobber
