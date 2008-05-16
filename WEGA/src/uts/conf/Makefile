
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
	sh -c 'for file in ${IFILES} ; do \
		cp $$file $(IDIR) ; \
		chmog $(MOD) $(OWN) $(GRP) $(IDIR)/$$file ; \
	done'
	chmog 666 wega system $(IDIR)/bstart.o

BOOTLIB: $(BL_OBJ) ../conf_times ../touch
# modify the objectfiles to make them as equal as possible to the
# original objects stored in BOOTLIB
	sh -c 'while read line ; do ../touch $$line ; done < ../conf_times'
	rm -f BOOTLIB
	ar qc BOOTLIB bmch.o boot.o bsys.o bconf.o bmisc.o bmd.o bfd.o bud.o brm.o

conf.o:
	scc $(CFLAGS) -c $<
