#
#	MAKEFILE ZUR INSTALLATION DER MM-DEMONSTRATIONEN
#	------------------------------------------------
#
#		(Alle Funkionen koennen auch mit grossen Buchstaben
#		 aufgerufen werden !)
#
#		Fuer die Demonstrationen wird die Datenbank `dt' benoetigt.
#


#
# -----	BASIS DEFINITIONEN
#
SHELL=	/bin/sh
AMX=	amx
CFLAGS=	-O
LDFLAGS=-s
CHMOD=	chmod
MODE=	0755
FORM=	../../lib/libform.a
RM=	amxrm -f
ALL=	formdemo							\
									\
	view								\
	v_erfind	v_lieferung	v_personal	v_teile		\
									\
	korr								\
	k_erfind	k_lieferung	k_personal	k_teile


#
# -----	INSTALL
#		fuehrt die Funktion(en)
#			formdemo
#			view
#			v_erfind
#			v_lieferung
#			v_personal
#			v_teile
#			korr
#			k_erfind
#			k_lieferung
#			k_personal
#			k_teile
#		zusammenhaengend aus.
#
INSTALL:	install

install:	$(ALL)


#
# -----	formdemo
#		Generierung Formdemonstration
#
formdemo:	formdemo.o $(FORM)
	$(CC) $(LDFLAGS) $@.o $(FORM) -o $@
	$(CHMOD) $(MODE) $@


#
# -----	view
#		Generierung Ansichtprogramm
#
view:		view.o $(FORM)
	$(CC) $(LDFLAGS) $@.o $(FORM) -o $@
	$(CHMOD) $(MODE) $@

#
# -----	v_erfind
#		Generierung Ansichtprogramm fuer die Relation erfind
#
v_erfind:	v_erfind.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)

#
# -----	v_lieferung
#		Generierung Ansichtprogramm fuer die Relation lieferung
#
v_lieferung:	v_lieferung.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)

#
# -----	v_personal
#		Generierung Ansichtprogramm fuer die Relation personal
#
v_personal:	v_personal.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)

#
# -----	v_teile
#		Generierung Ansichtprogramm fuer die Relation teile
#
v_teile:	v_teile.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)


#
# -----	korr
#		Generierung Korrekturprogramm
#
korr:		korr.o $(FORM)
	$(CC) $(LDFLAGS) $@.o $(FORM) -o $@
	$(CHMOD) $(MODE) $@

#
# -----	k_erfind
#		Generierung Korrekturprogramm fuer die Relation erfind
#
k_erfind:	k_erfind.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)

#
# -----	k_lieferung
#		Generierung Korrekturprogramm fuer die Relation lieferung
#
k_lieferung:	k_lieferung.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)

#
# -----	k_personal
#		Generierung Korrekturprogramm fuer die Relation personal
#
k_personal:	k_personal.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)

#
# -----	k_teile
#		Generierung Korrekturprogramm fuer die Relation teile
#
k_teile:	k_teile.o
	$(AMX) $(LDFLAGS) -o $@ $@.o $(FORM)


#
# -----	Definition der Abhaengigkeit zwischen den `.x'-Files
#	und den `.o'-Files (Generierungsvorschrift).
#
.x.o:
	$(AMX) -c $(CFLAGS) $<


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
HOME=	/z/dbs/demo/form
DISK=	/dev/dbs
SRC=	formdemo.c	k_erfind.x	k_lieferung.x	k_personal.x	\
	k_teile.x	korr.c		v_erfind.x	v_lieferung.x	\
	v_personal.x	v_teile.x	view.c		READ_ME		\
	Makefile

.save:		$(SRC)
	mount $(DISK) /wrk
	cp $? $@ /wrk/demo_form
	> $@
	umount $(DISK)

$(HOME)/.save:	$(SRC)
	cp $? $(HOME)
	> $@

.PRECIOUS:	.save
.PRECIOUS:	$(HOME)/.save
