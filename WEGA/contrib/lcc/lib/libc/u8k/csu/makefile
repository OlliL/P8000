#

SHELL=/bin/sh
CC=	lcc
CFLAGS=	-O

OFILES=		lcrt0.o
FILES=		$(OFILES)

all:		$(FILES)

#-------------------------

.s.o:
	@echo $*.s:
	$(CC) $(CFLAGS) -c $*.s

#-------------------------

clean:
		rm -f *.o

#--------------------------

clobber:	clean
