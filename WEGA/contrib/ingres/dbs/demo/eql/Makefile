#
#	MAKEFILE ZUR INSTALLATION DER EQL-DEMONSTRATIONEN
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
#			EQLDEMO
#			EQLTUT
#		zusammenhaengend aus
#
# -----	make EQLDEMO
#		generiert das einfache EQL-Beispielprogramm
#
# -----	make EQLTUT
#		generiert das EQL-Tutorial
#
# -----	make CLEAN
#		loescht alle generierten und installierten Beispiele.
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
EQC=	eqc
LIBQ=	../../lib/libq.a
RM=	rm -f


#
# -----	INSTALL
#		fuehrt die Funktion(en)
#			EQLDEMO
#			EQLTUT
#		zusammenhaengend aus
#
INSTALL:	install

install:	eqldemo eqltut


#
# -----	EQLDEMO
#		generiert das einfache EQL-Beispielprogramm.
#		Das einfache EQL-Beispiel `eqldemo.q' wird mit dem
#		EQL-Praecompiler `eqc' uebersetzt, der das File
#		`eqldemo.c' erzeugt. Dieses File wird mit dem
#		C-Compiler uebersetzt und mit der EQL-Laufzeit-
#		bibliothek `$(LIBQ)' (normalerweise `-lq')
#		gelinkt und kann durch den Aufruf von `eqldemo'
#		zur Ausfuehrung gebracht werden.
#
EQLDEMO:	eqldemo

eqldemo:	eqldemo.q
	$(EQC) eqldemo.q
	$(CC) eqldemo.c $(LIBQ) -o $@
	@-$(RM) eqldemo.c


#
# -----	EQLTUT
#		generiert das EQL-Tutorial.
#		Das EQL-Tutorial `eqltut.q' wird mit dem
#		EQL-Praecompiler `eqc' uebersetzt, der das File
#		`eqltut.c' erzeugt. Dieses File wird mit dem
#		C-Compiler uebersetzt und mit der EQL-Laufzeit-
#		bibliothek `$(LIBQ)' (normalerweise `-lq')
#		gelinkt und kann durch den Aufruf von `eqltut'
#		zur Ausfuehrung gebracht werden.
#
EQLTUT:	eqltut

eqltut:	eqltut.q
	$(EQC) eqltut.q
	$(CC) eqltut.c $(LIBQ) -o $@
	@-$(RM) eqltut.c


#
# -----	make CLEAN
#		loescht alle generierten und installierten Beispiele.
#
CLEAN:		clean

clean:
	@-$(RM) *.c eqldemo eqltut


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo/eql
DISK=	/dev/dbs
SRC=	eqldemo.q	eqltut.q	Makefile	READ_ME

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_eql
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
