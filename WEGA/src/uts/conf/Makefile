
AS=	as
CC=	cc
CFLAGS= -O


BL_OBJ= bconf.o bfd.o bmch.o bmd.o bmisc.o boot.o brm.o bsys.o bud.o
OBJ=	BOOTLIB bstart.o conf.o event.o fpe.o mch.o u.o

IFILES=	BOOTLIB bstart.c bstart.o conf.o event.o fpe.o make.boot make.wega \
	mch.o u.o ver.c+ wpar.c
IDIR=	/usr/sys/conf

OWN=	bin
GRP=	system
MOD=	644

all:	$(OBJ)

install: $(OBJ)
	sh -c 'test -d $(IDIR) || mkdir $(IDIR)'
	sh -c 'for file in ${IFILES} ; do \
		cp $$file $(IDIR) ; \
		chmog $(MOD) $(OWN) $(GRP) $(IDIR)/$$file ; \
	done'
	chmog 666 wega system $(IDIR)/bstart.o

BOOTLIB: $(BL_OBJ)
	ar qc BOOTLIB bmch.o boot.o bsys.o bconf.o bmisc.o bmd.o bfd.o bud.o brm.o

libb.a:	sa.function.o bmisc.o
	rm -f libb.a
	ar qc libb.a sa.function.o bmisc.o
	chmod 644 libb.a

conf.o:
	scc $(CFLAGS) -c $<
u.o:
	cp u.obj u.o

clean:
	rm -f $(BL_OBJ) $(OBJ)
