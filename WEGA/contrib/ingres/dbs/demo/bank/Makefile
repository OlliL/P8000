HOME=	/z/dbs/demo/bank
SHELL=	/bin/sh
DISK=	/dev/dbs

AMX=	amx
CFLAGS=	-O
LDFLAGS=-i
RM=	amxrm -f
CHMOD=	chmod
MODE=	0644

XSRC=	auszug.x	bank.x		buchung.x	grund.x		\
	konto.x		kunde.x		mahnung.x	menue.x		\
	name.x		ort.x		relations.x	str.x		\
	vorname.x	

CSRC=	cursor.c	print.c		util.c

OBJ=	menue.o		relations.o	util.o		auszug.o	\
	bank.o		buchung.o	cursor.o	grund.o		\
	konto.o		kunde.o		mahnung.o	name.o		\
	ort.o		print.o		str.o		vorname.o	

SRC=	bank.ascii	grund.ascii	konto.ascii	kunde.ascii	\
	name.ascii	ort.ascii	str.ascii	vorname.ascii	\
	$(CSRC)		$(XSRC)		cursor.h	Makefile

install:	bank reorg menue

bank:
	-@destroydb -s bank
	creatdb -1 -q bank
	creater	bank vorname			\
			no_vorname	i2	\
			vorname		c35
	copyr	bank vorname			\
			no_vorname	c0	\
			vorname		c0	\
		from vorname.ascii
	creater	bank name			\
			no_name		i2	\
			name		c35
	copyr	bank name			\
			no_name		c0	\
			name		c0	\
		from name.ascii
	creater	bank ort			\
			no_ort		i2	\
			plz		i2	\
			ort		c35
	copyr	bank ort			\
			no_ort		c0	\
			plz		c0	\
			ort		c0	\
		from ort.ascii
	creater	bank str			\
			no_str		i2	\
			str		c35
	copyr	bank str			\
			no_str		c0	\
			str		c0	\
		from str.ascii
	creater	bank bank			\
			no_bank		i2	\
			no_str		i2	\
			no_ort		i2	\
			bank		c35
	copyr	bank bank			\
			no_bank		c0	\
			no_str		c0	\
			no_ort		c0	\
			bank		c0	\
		from bank.ascii
	creater	bank kunde			\
			konto		i4	\
			no_name		i2	\
			no_vorname	i2	\
			no_str		i2	\
			no_ort		i2
	copyr	bank kunde			\
			konto		c0	\
			no_name		c0	\
			no_vorname	c0	\
			no_str		c0	\
			no_ort		c0	\
		from kunde.ascii
	creater	bank konto			\
			konto		i4	\
			saldo		i4	\
			datum		i4	\
			no_bank		i2
	copyr	bank konto			\
			konto		c0	\
			saldo		c0	\
			datum		c0	\
			no_bank		c0	\
		from konto.ascii
	creater	bank grund			\
			no_grund	i2	\
			plus		i2	\
			grund		c35
	copyr	bank grund			\
			no_grund	c0	\
			plus		c0	\
			grund		c0	\
		from grund.ascii
	creater	bank buchung			\
			konto		i4	\
			datum		i4	\
			betrag		i2	\
			no_grund	i2

reorg:
	restore -a bank
	modifyr	bank vorname			\
		chash   -minpages:13		\
			no_vorname
	modifyr	bank name			\
		chash   -minpages:13		\
			no_name	
	modifyr	bank ort			\
		chash   -minpages:7		\
			no_ort	
	modifyr	bank str			\
		chash   -minpages:7		\
			no_str
	modifyr	bank bank			\
		chash   -minpages:7		\
			no_bank
	modifyr	bank kunde			\
		hash    -minpages:13		\
			konto
	modifyr	bank konto			\
		hash    -minpages:31		\
			konto
	modifyr	bank grund			\
		chash	-maxpages:4
	modifyr	bank buchung			\
		hash    -minpages:127		\
			konto
	indexr	bank kunde index_kunde		\
		-nhash  -minpages:13 -tidp	\
			no_name			\
			no_vorname		\
			no_str			\
			no_ort
	indexr	bank konto index_konto		\
		-nhash  -minpages:7  -tidp	\
			no_bank

menue:		$(OBJ)
	$(AMX) $(LDFLAGS) -o $@ $(OBJ)

cursor.o:	cursor.h

auszug.o:	menue.x

bank.o:		menue.x

buchung.o:	menue.x

grund.o:	menue.x

konto.o:	menue.x

kunde.o:	menue.x

mahnung.o:	menue.x

name.o:		menue.x

ort.o:		menue.x

relations.o:	menue.x

str.o:		menue.x

vorname.o:	menue.x

.x.o:
	$(AMX) -c $(CFLAGS) $<

.SUFFIXES:	.o .x

clean:
	$(RM) menue *.o

INSTALL:	install

BANK:		bank

REORG:		reorg

MENUE:		menue

CLEAN:		clean

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_bank
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
