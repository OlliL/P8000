#
# makefile for libm
#
# Note that the object list given in FILES is in the order in which the files
# should be added to the archive when it is created.  If the files are not
# ordered correctly, linking of libm fails.

SHELL=/bin/sh

OWN=	wega
GRP=	system
PROT=	644

ROOT=	/z/LCC

LIB=$(ROOT)/lib
VARIANT =
LIBNAME = Llib$(VARIANT)m.a
CC=lcc
AR=ar
FILES =\
	$(LIBNAME)(asin.o)\
	$(LIBNAME)(atan.o)\
	$(LIBNAME)(erf.o)\
	$(LIBNAME)(fabs.o)\
	$(LIBNAME)(floor.o)\
	$(LIBNAME)(fmod.o)\
	$(LIBNAME)(gamma.o)\
	$(LIBNAME)(hypot.o)\
	$(LIBNAME)(jn.o)\
	$(LIBNAME)(j0.o)\
	$(LIBNAME)(j1.o)\
	$(LIBNAME)(pow.o)\
	$(LIBNAME)(log.o)\
	$(LIBNAME)(sin.o)\
	$(LIBNAME)(sqrt.o)\
	$(LIBNAME)(tan.o)\
	$(LIBNAME)(tanh.o)\
	$(LIBNAME)(sinh.o)\
	$(LIBNAME)(exp.o)\
	$(LIBNAME)(matherr.o)

all: $(LIBNAME)

$(LIBNAME): $(FILES)
	$(CC) -c $(CFLAGS) $(?:.o=.c)
	chmod $(PROT) $?
	chgrp $(GRP)  $?
	chown $(OWN)  $?
	$(AR) rv $(LIBNAME) $?
	rm $?

.c.a:;

install: all save move

clean:
	-rm -f *.o

clobber:	clean
	-rm -f $(LIBNAME)

move:
	rm -f $(LIB)/$(LIBNAME)
	cp $(LIBNAME) $(LIB)/$(LIBNAME) ; rm -f $(LIBNAME)
	chmod $(PROT) $(LIB)/$(LIBNAME)
	chmod -x $(LIB)/$(LIBNAME)
	chgrp $(GRP) $(LIB)/$(LIBNAME)
	chown $(OWN) $(LIB)/$(LIBNAME)

#--------------------------

uninstall:	$(LIB)/OLD$(LIBNAME)
		
		mv -f $(LIB)/OLD$(LIBNAME) $(LIB)/$(LIBNAME)lib$(VARIANT)c.a

#--------------------------

save:		
	if test -f $(LIB)/$(LIBNAME) ; \
	then \
		rm -f $(LIB)/OLD$(LIBNAME) ; \
		cp $(LIB)/$(LIBNAME) $(LIB)/OLD$(LIBNAME) ; \
	fi
