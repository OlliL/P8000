#
#	MAKEFILE ZUR INSTALLATION DER REPORT-DEMONSTRATIONEN
#	----------------------------------------------------
#
#		(Alle Funkionen koennen auch mit grossen Buchstaben
#		 aufgerufen werden !)
#
#	FUNKTIONSBESCHREIBUNG
#	---------------------
#
# -----	make INSTALL
#		fuehrt die Funktion(en)
#			MITARBEITER
#			ERFINDUNGEN
#			PERSONAL
#		zusammenhaengend aus
#
# -----	make MITARBEITER
#		generiert die Angestelltenliste
#
# -----	make ERFINDUNGEN
#		generiert die Erfindungsliste
#
# -----	make PERSONAL
#		generiert die Personalliste
#
# -----	make CLEAN
#		loescht alle generierten und installierten Beispiele.
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
RM=	rm -f


#
# -----	INSTALL
#		fuehrt die Funktion(en)
#			MITARBEITER
#			ERFINDUNGEN
#			PERSONAL
#		zusammenhaengend aus
#
INSTALL:	install

install:	mitarbeiter erfindungen personal


#
# -----	MITARBEITER
#		generiert die Angestelltenliste aus der Quelle `report01'.
#		Die Quelle wird mit dem Reportcompiler `rc' uebersetzt,
#		`rc' erzeugt den Interpretercode `r01' und die
#		Beschreibung `d01'.
#		Dem Reportgenerator `rg' werden diese Files und die
#		Daten `tuple01' zur Erstellung der Liste uebergeben.
#		Die Angabe von `-a24' und `-l14' erzeugt eine Liste
#		die dem Bildschirmformat angepasst ist.
#		Anschliessend werden die erzeugten Files geloescht.
#
MITARBEITER:	mitarbeiter

mitarbeiter:
	rc report01
	rg -a24 -l14 -ir01 -dd01 -ftuple01
	@-$(RM) ?01


#
# -----	ERFINDUNGEN
#		generiert die Erfindungsliste aus der Quelle `report02'.
#		Die Quelle wird mit dem Reportcompiler `rc' uebersetzt,
#		`rc' erzeugt den Interpretercode `r02' und die
#		Beschreibung `d02'.
#		Die zu druckende Relation `erfind' wird entsprechend
#		dem Report sortiert und in das File `t02' kopiert.
#		Dem Reportgenerator `rg' werden diese Files und die
#		Daten `t02' zur Erstellung der Liste uebergeben.
#		Die Angabe von `-a24' und `-l14' erzeugt eine Liste
#		die dem Bildschirmformat angepasst ist.
#		Anschliessend werden die erzeugten Files geloescht.
#
ERFINDUNGEN:	erfindungen

erfindungen:
	rc report02
	modifyr dt erfind heapsort land erfinder erfindung jahr
	copyr dt erfind land c12 erfinder c20 erfindung c36 jahr c9 into t02
	modifyr dt erfind cisam erfindung
	rg -a24 -l14 -ir02 -dd02 -ft02
	@-$(RM) ?02


#
# -----	PERSONAL
#		generiert die Personalliste aus der Quelle `report03'.
#		Die Quelle wird mit dem Reportcompiler `rc' uebersetzt,
#		`rc' erzeugt den Interpretercode `r03' und die
#		Beschreibung `d03'.
#		Die zu druckende Relation `personal' wird entsprechend
#		dem Report sortiert und in das File `t03' kopiert.
#		Dem Reportgenerator `rg' werden diese Files und die
#		Daten `t03' zur Erstellung der Liste uebergeben.
#		Die Angabe von `-a24' und `-l14' erzeugt eine Liste
#		die dem Bildschirmformat angepasst ist.
#		Anschliessend werden die erzeugten Files geloescht.
#
PERSONAL:	personal

personal:
	rc report03
	modifyr dt personal heapsort leiter
	copyr dt personal into t03
	modifyr dt personal cisam num
	rg -a24 -l14 -ir03 -dd03 -ft03
	@-$(RM) ?03


# -----	make CLEAN
#		loescht alle generierten und installierten Beispiele.
#
CLEAN:		clean

clean:
	@-$(RM) [drt][0-9][0-9] r_code desc tuple


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo/pqr
DISK=	/dev/dbs
SRC=	report01	report02	report03	tuple01		\
	Makefile	READ_ME

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_pqr
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
