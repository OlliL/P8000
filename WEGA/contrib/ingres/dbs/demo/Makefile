#
#	MAKEFILE ZUR INSTALLATION DER DEMONSTRATIONEN
#	---------------------------------------------
#
#		(Alle Funkionen koennen auch mit grossen Buchstaben
#		 aufgerufen werden !)
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
MAKE=	make
MKFLAGS=-k


#
# -----	INSTALL
#		fuehrt die Funktion(en)
#			DT
#			AMX_DEMO
#			DBSR_DEMO
#			EQL_DEMO
#			FORM_DEMO
#			PQR_DEMO 
#			QL_DEMO
#		zusammenhaengend aus
#
INSTALL:	install

install:	dt amx_demo dbsr_demo eql_demo form_demo pqr_demo ql_demo


#
# -----	DT
#		generiert die fuer die Demonstration(en)
#		benoetigte Datenbank `dt' mit den Relationen
#			`personal'
#			`teile'
#			`lieferung'
#			`erfind'
#		Es werden Anfangsdaten geladen und die
#		Systemkataloge der Datenbank `dt' geschluesselt.
#
DT:		dt

dt:
	-@destroydb -s dt
	creatdb -1 -c -q dt
	creater	dt personal			\
			num		i2	\
			name		c20	\
			gehalt		i2	\
			leiter		i2	\
			gebdat		i2	\
			einstdat	i2
	copyr	dt personal			\
			num		c0	\
			name		c0	\
			gehalt		c0	\
			leiter		c0	\
			gebdat		c0	\
			einstdat	c0	\
		from ql/personal
	modifyr dt personal cisam num
	creater	dt teile			\
			tnum		i2	\
			tname		c20	\
			farbe		c8	\
			gewicht		i2	\
			menge		i2
	copyr	dt teile			\
			tnum		c0	\
			tname		c0	\
			farbe		c0	\
			gewicht		c0	\
			menge		c0	\
		from ql/teile
	modifyr dt teile cisam tnum
	creater	dt lieferung			\
			bnum		i2	\
			tnum		i2	\
			lnum		i2	\
			liefdat		c8	\
			menge		i2
	copyr	dt lieferung			\
			lnum		c0	\
			tnum		c0	\
			bnum		c0	\
			liefdat		c0	\
			menge		c0	\
		from ql/lieferung
	modifyr dt lieferung isam bnum tnum
	indexr dt lieferung index_lnum lnum
	creater dt erfind			\
			erfindung	c36	\
			erfinder	c20	\
			jahr		c9	\
			land		c12
	copyr	dt erfind			\
			x0		"d0'"	\
			erfindung	"c0'"	\
			x1		d2	\
			erfinder	"c0'"	\
			x2		"d0'"	\
			jahr		"c0'"	\
			x3		d2	\
			land		"c0'"	\
			x4		"d0;"	\
			x5		d0nl	\
		from ql/erfindungen
	modifyr dt erfind cisam erfindung
	sysmod	dt


#
# -----	AMX_DEMO
#		generiert die AMX-Beispielprogramme, die in dem
#		Verzeichnis `.../demo/amx' abgelegt werden. Sie
#		koennen dort von dem lernenden Nutzer abgearbeitet
#		werden. Die Programme heissen `01' bis `18'. Fuer
#		die Ausfuehrung der Generierung wird die Daten-
#		bank `dt' vorausgesetzt (siehe `.../demo/amx/READ_ME').
#
AMX_DEMO:	amx_demo

amx_demo:
	cd amx; $(MAKE) $(MKFLAGS) install


#
# -----	DBSR_DEMO
#		generiert AMX-Beispielprogramme fuer die Mengen-
#		bedarfsermittlung (Stuecklistenaufloesung), die
#		in dem Verzeichnis `.../demo/dbsr' abgelegt werden.
#
DBSR_DEMO:	dbsr_demo

dbsr_demo:
	cd dbsr; $(MAKE) $(MKFLAGS) dbsr install


#
# -----	FORM_DEMO
#		generiert die FORM-Beispielprogramme, die in dem
#		Verzeichnis `.../demo/eql' abgelegt werden. Sie
#		koennen dort von dem lernenden Nutzer abgearbeitet
#		werden. Die Programme heissen `eqldemo' und `eqltut'.
#		Fuer die Abarbeitung wird die Datenbank `dt' vorausgesetzt.
#
FORM_DEMO:	eql_demo

eql_demo:
	cd eql; $(MAKE) $(MKFLAGS) install


#
# -----	FORM_DEMO
#		generiert die FORM-Beispielprogramme, die in dem
#		Verzeichnis `.../demo/form' abgelegt werden. Sie
#		koennen dort von dem lernenden Nutzer abgearbeitet
#		werden. Das Programm heisst `formdemo'.
#		Fuer die Abarbeitung wird die Datenbank `dt' vorausgesetzt.
#
FORM_DEMO:	form_demo

form_demo:
	cd form; $(MAKE) $(MKFLAGS) install


#
# -----	PQR_DEMO
#		generiert die Beispiele zum Reportsystem in dem
#		Verzeichnis `.../demo/pqr'. Sie koennen dort
#		abgearbeitet werden (siehe `.../demo/pqr/READ_ME').
#
PQR_DEMO:	pqr_demo

pqr_demo:
	cd pqr; $(MAKE) $(MKFLAGS) install


#
# -----	QL_DEMO
#		installiert das Kommando `demodb' zur Erzeugung
#		einer vollstaendigen Demonstrationsdatenbank
#		durch einen Datenbanknutzer oeffentlich.
#		Zusaetzlich wird das Kommando `tutdb' zum auto-
#		matischen Abarbeiten der QL-Lehrbeispiele.
#		Beide Kommandos erhalten als Argument ein vom
#		Nutzer vorzuschlagenden Datenbanknamen.
#
QL_DEMO:	ql_demo

ql_demo:
	cd ql; $(MAKE) $(MKFLAGS) install


#
# -----	CLEAN
#		loescht alle generierten und installierten Beispiele.
#
CLEAN:		clean

clean:
	cd amx; $(MAKE) $(MKFLAGS) clean
	cd dbsr; $(MAKE) $(MKFLAGS) clean
	cd eql; $(MAKE) $(MKFLAGS) clean
	cd pqr; $(MAKE) $(MKFLAGS) clean
	cd ql; $(MAKE) $(MKFLAGS) clean


#
#	FOLGENDE FUNKTION SIND INTERN
#
HOME=	/z/dbs/demo
SRC=	Makefile	READ_ME
DISK=	/dev/dbs

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
