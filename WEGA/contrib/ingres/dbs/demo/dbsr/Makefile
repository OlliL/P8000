SHELL=	/bin/sh

CC=	amx
CFLAGS=	-O
LDFLAGS=-s
RM=	amxrm -f

ALL=	stl stl1 tot

INSTALL:	install

install:	$(ALL)

DBSR:		dbsr

dbsr:
	-@destroydb -s dbsr
	creatdb -1 -c -q dbsr
	creater dbsr teile			\
			kenn		c1	\
			t		c13	\
			name		c30	\
			me		c2	\
			m		i2
	copyr dbsr teile			\
			kenn		c0	\
			t		c0	\
			name		c0	\
			me		c0	\
			m		c0nl	\
		from teile.ascii
	modifyr	dbsr teile			\
		cisam				\
			t
	creater dbsr inf			\
			dummy1		c2	\
			ot		c13	\
			ut		c13	\
			dummy2		i2	\
			dummy3		i2	\
			m		i2
	copyr dbsr inf				\
			dummy1		c0	\
			ot		c0	\
			ut		c0	\
			dummy2		c0	\
			dummy3		c0	\
			m		c0nl	\
		from inf.ascii
	modifyr	dbsr inf			\
		isam				\
			ot			\
			ut
	indexr	dbsr inf index_inf		\
		-nisam				\
			ut
	creater dbsr total			\
			lev		i2	\
			t		c13	\
			name		c30	\
			menge		i2	\
			me		c2
	sysmod	dbsr

OT:		ot

ot:		ot.o
	$(CC) $(LDFLAGS) -o $@ $@.o

OT_T:		ot_t

ot_t:		ot_t.o
	$(CC) $(LDFLAGS) -o $@ $@.o

OTA:		ota

ota:		ota.o
	$(CC) $(LDFLAGS) -o $@ $@.o

OTBC:		otbc

otbc:		otbc.o
	$(CC) $(LDFLAGS) -o $@ $@.o

STL:		stl

stl:		stl.o
	$(CC) $(LDFLAGS) -o $@ $@.o

STL1:		stl1

stl1:		stl1.o
	$(CC) $(LDFLAGS) -o $@ $@.o

TOT:		tot

tot:		tot.o
	$(CC) $(LDFLAGS) -o $@ $@.o


UT_T:		ut_t

ut_t:		ut_t.o
	$(CC) $(LDFLAGS) -o $@ $@.o

.x.o:
	$(CC) -c $(CFLAGS) $<

.SUFFIXES:	.o .x

CLEAN:		clean

clean:
	$(RM) $(ALL) *.o


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo/dbsr
DISK=	/dev/dbs
SRC=	READ_ME		inf.ascii	ot.x		ot_t.x		\
	ota.x		otbc.x		stl.x		stl1.x		\
	teile.ascii	tot.x		ut_t.x		Makefile

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_dbsr
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
