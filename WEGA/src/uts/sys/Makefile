
CAS=	cas
AS=	as
CC=	scc
CFLAGS= -O


OBJ=	main.o trap.o sig.o iget.o prf.o slp.o subr.o rdwri.o fio.o malloc.o \
	alloc.o machdep.o nami.o pipe.o sysent.o sys3.o sys1.o sys4.o sys2.o \
	acct.o text.o ureg.o break.o lock.o mkseg.o debug.o timer.o 


IDIR=	/usr/sys/sys

OWN=	bin
GRP=	system
MOD=	644

all:	LIB1

install: LIB1
	sh -c 'test -d $(IDIR) || mkdir $(IDIR)'
	cp LIB1 $(IDIR)
	chmog $(MOD) $(OWN) $(GRP) $(IDIR)/LIB1

LIB1:	$(OBJ)
	ar cr LIB1 $(OBJ)

# No source available for this files
lock.o:
	cp lock.obj lock.o

clean:
	rm -f $(OBJ) LIB1
