#
#	MAKEFILE ZUR INSTALLATION DER AMX-DEMONSTRATIONEN
#	-------------------------------------------------
#
#		(Alle Funkionen koennen auch mit grossen Buchstaben
#		 aufgerufen werden !)
#
#	FUNKTIONSBESCHREIBUNG
#	---------------------
#
# -----	make INSTALL
#		fuehrt die Funktion(en)
#			01
#			02
#			03
#			04
#			05
#			06
#			07
#			08
#			09
#			10
#			11
#			12
#			13
#			14
#			15
#			16
#			17
#			18
#		zusammenhaengend aus.
#		Fuer die Demonstrationen wird die Datenbank `dt' benoetigt.
#
# -----	make 01
#		Generierung AMX-Beispielprogramm 01
#
# -----	make 02
#		Generierung AMX-Beispielprogramm 02
#
# -----	make 03
#		Generierung AMX-Beispielprogramm 03
#
# -----	make 04
#		Generierung AMX-Beispielprogramm 04
#
# -----	make 05
#		Generierung AMX-Beispielprogramm 05
#
# -----	make 06
#		Generierung AMX-Beispielprogramm 06
#
# -----	make 07
#		Generierung AMX-Beispielprogramm 07
#
# -----	make 08
#		Generierung AMX-Beispielprogramm 08
#
# -----	make 09
#		Generierung AMX-Beispielprogramm 09
#
# -----	make 10
#		Generierung AMX-Beispielprogramm 10
#
# -----	make 11
#		Generierung AMX-Beispielprogramm 11
#
# -----	make 12
#		Generierung AMX-Beispielprogramm 12
#
# -----	make 13
#		Generierung AMX-Beispielprogramm 13
#
# -----	make 14
#		Generierung AMX-Beispielprogramm 14
#
# -----	make 15
#		Generierung AMX-Beispielprogramm 15
#
# -----	make 16
#		Generierung AMX-Beispielprogramm 16
#
# -----	make 17
#		Generierung AMX-Beispielprogramm 17
#
# -----	make 18
#		Generierung AMX-Beispielprogramm 18
#
# -----	make CLEAN
#		loescht alle generierten und installierten Beispiele.
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
CC=	amx
CFLAGS=	-O
LDFLAGS=-s
RM=	amxrm -f
ALL=	01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18


#
# -----	INSTALL
#		fuehrt die Funktion(en)
#			01
#			02
#			03
#			04
#			05
#			06
#			07
#			08
#			09
#			10
#			11
#			12
#			13
#			14
#			15
#			16
#			17
#			18
#		zusammenhaengend aus.
#
INSTALL:	install

install:	$(ALL)


#
# -----	01
#		Generierung AMX-Beispielprogramm 01
#	retrieve (personal.all) where personal.name = "arg"
#
01:		01.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	02
#		Generierung AMX-Beispielprogramm 02
#	retrieve (personal.name) where personal.name = "arg"
#
02:		02.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	03
#		Generierung AMX-Beispielprogramm 03
#	retrieve (personal.name, seinen Leiter)
#
03:		03.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	04
#		Generierung AMX-Beispielprogramm 04
#	retrieve (personal.name, alle seine Leiter)
#
04:		04.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	05
#		Generierung AMX-Beispielprogramm 05
#	retrieve (personal.name, seine Stellung)
#
05:		05.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	06
#		Generierung AMX-Beispielprogramm 06
#	retrieve (personal.Struktur)
#
06:		06.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	07
#		Generierung AMX-Beispielprogramm 07
#	retrieve (teile.name, teile.farbe) where teile.farbe = "grau"
#
07:		07.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	08
#		Generierung AMX-Beispielprogramm 08
#	retrieve (teile.tname, teile.tnum, tot = teile.menge * teile.gewicht)
#
08:		08.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	09
#		Generierung AMX-Beispielprogramm 09
#	retrieve (teile.tname, teile.tnum)
#	where teile.tnum = 2 or teile.tnum = 10
#
09:		09.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	10
#		Generierung AMX-Beispielprogramm 10
#	retrieve (teile.tname, teile.tnum)
#	where teile.tnum > 5 and teile.tnum <= 10
#
10:		10.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	11
#		Generierung AMX-Beispielprogramm 11
#	retrieve (lieferung.tnum) where lieferung.lnum = 122
#
11:		11.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	12
#		Generierung AMX-Beispielprogramm 12
#	retrieve (teile.tname)
#	where lieferung.lnum = 122 and lieferung.tnum = teile.tnum
#
12:		12.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	13
#		Generierung AMX-Beispielprogramm 13
#	retrieve (lieferung.lnum)
#	where teile.tname = "Zentraleinheit" and lieferung.tnum = teile.tnum
#
13:		13.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	14
#		Generierung AMX-Beispielprogramm 14
#	delete personal
#
14:		14.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	15
#		Generierung AMX-Beispielprogramm 15
#	delete personal where personal.leiter = 129
#
15:		15.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	16
#		Generierung AMX-Beispielprogramm 16
#	append personal (name = "Unfug, Ulli", gehalt = 10000)
#
16:		16.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	17
#		Generierung AMX-Beispielprogramm 17
#	replace personal (num = 12, leiter = 157)
#	where personal.name = "Unfug, Ulli"
#
17:		17.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	18
#		Generierung AMX-Beispielprogramm 18
#	replace personal (gehalt = 1.1 * personal.gehalt)
#
18:		18.o
	$(CC) $(LDFLAGS) -o $@ $@.o


#
# -----	Definition der Abhaengigkeit zwischen den `.x'-Files
#	und den `.o'-Files (Generierungsvorschrift).
#
.x.o:
	$(CC) -c $(CFLAGS) $<


#
# -----	Definition der zulaessigen Filenamenendungen
#
.SUFFIXES:	.o .x


#
# -----	CLEAN
#		loescht alle generierten und installierten Beispiele.
#
CLEAN:		clean

clean:
	$(RM) $(ALL) *.o


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo/amx
DISK=	/dev/dbs
SRC=	01.x		02.x		03.x		04.x		\
	05.x		06.x		07.x		08.x		\
	09.x		10.x		11.x		12.x		\
	13.x		14.x		15.x		16.x		\
	17.x		18.x		READ_ME		Makefile


.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_amx
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
