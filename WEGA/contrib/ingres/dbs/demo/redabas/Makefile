#
#	MAKEFILE ZUR GENERIERUNG DER REDABAS-Datenbanken
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
#			REDABAS4
#			REDABAS3
#			REDABAS2
#		zusammenhaengend aus.
#		Fuer die Demonstrationen wird die Datenbank `dt' benoetigt.
#
# -----	make REDABAS4
#		fuehrt die Funktion(en)
#			ABT4.DBD
#			ARTIKEL4.DBD
#			LIEFERA4.DBD
#			LIEFERU4.DBD
#			PERFIND4.DBD
#			PERSONAL4.DBD
#			LAGER4.DBD
#			TEILE4.DBD
#			VERKAUF4.DBD
#		zusammenhaengend aus.
#
# -----	make REDABAS3
#		fuehrt die Funktion(en)
#			ABT3.DBD
#			ARTIKEL3.DBD
#			LIEFERA3.DBD
#			LIEFERU3.DBD
#			PERFIND3.DBD
#			PERSONAL3.DBD
#			LAGER3.DBD
#			TEILE3.DBD
#			VERKAUF3.DBD
#		zusammenhaengend aus.
#
# -----	make REDABAS2
#		fuehrt die Funktion(en)
#			ABT2.DBD
#			ARTIKEL2.DBD
#			LIEFERA2.DBD
#			LIEFERU2.DBD
#			PERFIND2.DBD
#			PERSONAL2.DBD
#			LAGER2.DBD
#			TEILE2.DBD
#			VERKAUF2.DBD
#		zusammenhaengend aus.
#
# -----	make ABT4.DBD
#		Erzeugt die REDABAS 4 Datenbank ABT4.DBD
#		aus der Relation abt
#
# -----	make ARTIKEL4.DBD
#		Erzeugt die REDABAS 4 Datenbank ARTIKEL4.DBD
#		aus der Relation artikel
#
# -----	make ERFIND4.DBD
#		Erzeugt die REDABAS 4 Datenbank ERFIND4.DBD
#		aus der Relation erfind
#
# -----	make LAGER4.DBD
#		Erzeugt die REDABAS 4 Datenbank LAGER4.DBD
#		aus der Relation lager
#
# -----	make LIEFERA4.DBD
#		Erzeugt die REDABAS 4 Datenbank LIEFERA4.DBD
#		aus der Relation lieferant
#
# -----	make LIEFERU4.DBD
#		Erzeugt die REDABAS 4 Datenbank LIEFERU4.DBD
#		aus der Relation lieferung
#
# -----	make PERS4.DBD
#		Erzeugt die REDABAS 4 Datenbank PERS4.DBD
#		aus der Relation personal
#
# -----	make TEILE4.DBD
#		Erzeugt die REDABAS 4 Datenbank TEILE4.DBD
#		aus der Relation teile
#
# -----	make VERKAUF4.DBD
#		Erzeugt die REDABAS 4 Datenbank VERKAUF4.DBD
#		aus der Relation verkauf
#
# -----	make ABT3.DBD
#		Erzeugt die REDABAS 3 Datenbank ABT3.DBD
#		aus der Relation abt
#
# -----	make ARTIKEL3.DBD
#		Erzeugt die REDABAS 3 Datenbank ARTIKEL3.DBD
#		aus der Relation artikel
#
# -----	make ERFIND3.DBD
#		Erzeugt die REDABAS 3 Datenbank ERFIND3.DBD
#		aus der Relation erfind
#
# -----	make LAGER3.DBD
#		Erzeugt die REDABAS 3 Datenbank LAGER3.DBD
#		aus der Relation lager
#
# -----	make LIEFERA3.DBD
#		Erzeugt die REDABAS 3 Datenbank LIEFERA3.DBD
#		aus der Relation lieferant
#
# -----	make LIEFERU3.DBD
#		Erzeugt die REDABAS 3 Datenbank LIEFERU3.DBD
#		aus der Relation lieferung
#
# -----	make PERS3.DBD
#		Erzeugt die REDABAS 3 Datenbank PERS3.DBD
#		aus der Relation personal
#
# -----	make TEILE3.DBD
#		Erzeugt die REDABAS 3 Datenbank TEILE3.DBD
#		aus der Relation teile
#
# -----	make VERKAUF3.DBD
#		Erzeugt die REDABAS 3 Datenbank VERKAUF3.DBD
#		aus der Relation verkauf
#
# -----	make ABT2.DBD
#		Erzeugt die REDABAS 2 Datenbank ABT2.DBD
#		aus der Relation abt
#
# -----	make ARTIKEL2.DBD
#		Erzeugt die REDABAS 2 Datenbank ARTIKEL2.DBD
#		aus der Relation artikel
#
# -----	make ERFIND2.DBD
#		Erzeugt die REDABAS 2 Datenbank ERFIND2.DBD
#		aus der Relation erfind
#
# -----	make LAGER2.DBD
#		Erzeugt die REDABAS 2 Datenbank LAGER2.DBD
#		aus der Relation lager
#
# -----	make LIEFERA2.DBD
#		Erzeugt die REDABAS 2 Datenbank LIEFERA2.DBD
#		aus der Relation lieferant
#
# -----	make LIEFERU2.DBD
#		Erzeugt die REDABAS 2 Datenbank LIEFERU2.DBD
#		aus der Relation lieferung
#
# -----	make PERS2.DBD
#		Erzeugt die REDABAS 2 Datenbank PERS2.DBD
#		aus der Relation personal
#
# -----	make TEILE2.DBD
#		Erzeugt die REDABAS 2 Datenbank TEILE2.DBD
#		aus der Relation teile
#
# -----	make VERKAUF2.DBD
#		Erzeugt die REDABAS 2 Datenbank VERKAUF2.DBD
#		aus der Relation verkauf
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
RM=	amxrm -f
WDB2=	wdb2
WDB3=	wdb3
WDB4=	wdb3 +
REDABAS4=	abt4.dbd	artikel4.dbd	erfind4.dbd	\
		lager4.dbd	liefera4.dbd	lieferu4.dbd	\
		pers4.dbd	teile4.dbd	verkauf4.dbd
REDABAS3=	abt3.dbd	artikel3.dbd	erfind3.dbd	\
		lager3.dbd	liefera3.dbd	lieferu3.dbd	\
		pers3.dbd	teile3.dbd	verkauf3.dbd
REDABAS2=	abt2.dbd	artikel2.dbd	erfind2.dbd	\
		lager2.dbd	liefera2.dbd	lieferu2.dbd	\
		pers2.dbd	teile2.dbd	verkauf2.dbd


INSTALL:	install

install:	redabas4 redabas3 redabas2

REDABAS4:	redabas4

redabas4:	$(REDABAS4)

REDABAS3:	redabas3

redabas3:	$(REDABAS3)

REDABAS2:	redabas2

redabas2:	$(REDABAS2)

ABT4.DBD:	abt4.dbd
abt4.dbd:	abt3
	$(WDB4) abt3
	@mv abt3.dbd $@

ARTIKEL4.DBD:	artikel4.dbd
artikel4.dbd:	artikel3
	$(WDB4) artikel3
	@mv artikel3.dbd $@

ERFIND4.DBD:	erfind4.dbd
erfind4.dbd:	erfind3
	$(WDB4) erfind3
	@mv erfind3.dbd $@

LAGER4.DBD:	lager4.dbd
lager4.dbd:	lager3
	$(WDB4) lager3
	@mv lager3.dbd $@

LIEFERA4.DBD:	liefera4.dbd
liefera4.dbd:	liefera3
	$(WDB4) liefera3
	@mv liefera3.dbd $@

LIEFERU4.DBD:	lieferu4.dbd
lieferu4.dbd:	lieferu3
	$(WDB4) lieferu3
	@mv lieferu3.dbd $@

PERS4.DBD:	pers4.dbd
pers4.dbd:	pers3
	$(WDB4) pers3
	@mv pers3.dbd $@

TEILE4.DBD:	teile4.dbd
teile4.dbd:	teile3
	$(WDB4) teile3
	@mv teile3.dbd $@

VERKAUF4.DBD:	verkauf4.dbd
verkauf4.dbd:	verkauf3
	$(WDB4) verkauf3
	@mv verkauf3.dbd $@

ABT3.DBD:	abt3.dbd
abt3.dbd:	abt3
	$(WDB3) abt3

ARTIKEL3.DBD:	artikel3.dbd
artikel3.dbd:	artikel3
	$(WDB3) artikel3

ERFIND3.DBD:	erfind3.dbd
erfind3.dbd:	erfind3
	$(WDB3) erfind3

LAGER3.DBD:	lager3.dbd
lager3.dbd:	lager3
	$(WDB3) lager3

LIEFERA3.DBD:	liefera3.dbd
liefera3.dbd:	liefera3
	$(WDB3) liefera3

LIEFERU3.DBD:	lieferu3.dbd
lieferu3.dbd:	lieferu3
	$(WDB3) lieferu3

PERS3.DBD:	pers3.dbd
pers3.dbd:	pers3
	$(WDB3) pers3

TEILE3.DBD:	teile3.dbd
teile3.dbd:	teile3
	$(WDB3) teile3

VERKAUF3.DBD:	verkauf3.dbd
verkauf3.dbd:	verkauf3
	$(WDB3) verkauf3

ABT2.DBD:	abt2.dbd
abt2.dbd:	abt2
	$(WDB2) abt2

ARTIKEL2.DBD:	artikel2.dbd
artikel2.dbd:	artikel2
	$(WDB2) artikel2

ERFIND2.DBD:	erfind2.dbd
erfind2.dbd:	erfind2
	$(WDB2) erfind2

LAGER2.DBD:	lager2.dbd
lager2.dbd:	lager2
	$(WDB2) lager2

LIEFERA2.DBD:	liefera2.dbd
liefera2.dbd:	liefera2
	$(WDB2) liefera2

LIEFERU2.DBD:	lieferu2.dbd
lieferu2.dbd:	lieferu2
	$(WDB2) lieferu2

PERS2.DBD:	pers2.dbd
pers2.dbd:	pers2
	$(WDB2) pers2

TEILE2.DBD:	teile2.dbd
teile2.dbd:	teile2
	$(WDB2) teile2

VERKAUF2.DBD:	verkauf2.dbd
verkauf2.dbd:	verkauf2
	$(WDB2) verkauf2

#
# -----	CLEAN
#		loescht alle generierten und installierten Beispiele.
#
CLEAN:		clean

clean:
	$(RM) $(REDABAS4) $(REDABAS3) $(REDABAS2)


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo/redabas
DISK=	/dev/dbs
SRC=		abt3		artikel3	erfind3		\
		lager3		liefera3	lieferu3	\
		pers3		teile3		verkauf3	\
		abt2		artikel2	erfind2		\
		lager2		liefera2	lieferu2	\
		pers2		teile2		verkauf2	\
		READ_ME		Makefile


.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_redabas
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
