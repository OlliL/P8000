#
#	MAKEFILE ZUR INSTALLATION DER DEMONSTRATIONEN
#	---------------------------------------------
#
#		(Alle Funkionen koennen auch mit grossen Buchstaben
#		 aufgerufen werden !)
#
#	FUNKTIONSBESCHREIBUNG
#	---------------------
#
# -----	make INSTALL
#		installiert das Kommando `demodb' zur Erzeugung
#		einer vollstaendigen Demonstrationsdatenbank
#		durch einen Datenbanknutzer oeffentlich.
#		Zusaetzlich wird das Kommando `tutdb' zum auto-
#		matischen Abarbeiten der QL-Lehrbeispiele erzeugt.
#
# -----	make CLEAN
#		loescht alle generierten und installierten Beispiele.
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
BIN=	../../bin
RM=	rm -f
CP=	cp
CHMOD=	chmod
MODE=	0755


#
# -----	INSTALL
#		installiert das Kommando `demodb' zur Erzeugung
#		einer vollstaendigen Demonstrationsdatenbank
#		durch einen Datenbanknutzer oeffentlich.
#		Zusaetzlich wird das Kommando `tutdb' zum auto-
#		matischen Abarbeiten der QL-Lehrbeispiele erzeugt.
#		Beide Kommandos erhalten als Argument ein vom
#		Nutzer vorzuschlagenden Datenbanknamen.
#
INSTALL:	install

install:	$(BIN)/demodb $(BIN)/tutdb

$(BIN)/demodb:	demodb
	$(CP) demodb $@
	@$(CHMOD) $(MODE) $@

$(BIN)/tutdb:	tutdb
	$(CP) tutdb $@
	@$(CHMOD) $(MODE) $@

#
# -----	CLEAN
#		loescht alle generierten und installierten Beispiele.
#
CLEAN:		clean

clean:
	@-$(RM) $(BIN)/demodb $(BIN)/tutdb


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo/ql
DISK=	/dev/dbs
SRC=	Makefile	READ_ME		abt		artikel		\
	erfindungen	lager		lieferant	lieferung	\
	personal	teile		verkauf		demodb		\
	tutdb		tutorial

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_ql
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
