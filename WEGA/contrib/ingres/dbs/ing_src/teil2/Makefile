# Bemerkungen
#
#  Es besteht die Moeglichkeit, mit dem Makefile die
#  Dokumentation von ING_DB fuer verschiedene Drucker
#  herzustellen.
#
#  In der Titelfestlegung kann bei 'T = ' noch eine
#  Option fuer 'nroff' gegeben werden, die speziell
#  fuer besondere Drucker aufbereitet (Je nach den 
#  'nroff'- Moeglichkeiten).
#
#  'doc2' schafft den ersten Teil der Dokumentation als
#  ganzes nach /z/ing_doc/teil2/doc2. Der Text ist nicht
#  speziell aufbereitet.
#  EINGABE   make doc2 bzw. make
#
#  'doc2.lp' bereitet den Text fuer normale Nadeldrucker
#  auf. Es sind Umlaute vorgesehen und "nroff" fuehrt
#  Fettdruck aus.
#  Es sind keine "tab's" gesetzt.
#  Der Teil1 des Handbuchs wird anschliessend in Bloecke zu
#  30 Seiten geteilt, im directory /z/ing_doc/teil2 abgelegt
#  und dort gedruckt.
#  EINGABE    make doc2.tx
#
#  'doc2.lp bereitet das Handbuch fuer Seriendrucker auf.
#  Es werden kein Fettdruck und keine Unterstreichungen
#  ausgefuehrt.
#  Ansonsten gilt dasselbe wie bei 'doc2.tx'.
#  EINGABE    make doc2.lp
#
#############################################################

M = /z/ing_doc/teil2/
S = /z/ing_src/teil2/lp/
D = /z/ing_src/teil2/
B = /z/ing_src/teil2/befehle/
C = /z/ing_doc/c/
T =

OBJS =	$(D)quel $(D)dab $(D)zugr $(D)monitor $(D)amx	\
	$(D)report1.7 $(D)report2.8 $(D)zip

doc2:	$(OBJS)
	nroff -ms $(OBJS) > $(M)doc2

doc2.tx: $(OBJS)
	 nroff -ms $(OBJS) > $(D)doc2.1
	 sed -f $(B)dbef $(D)doc2.1 > $(D)doc2.2
	 $(D)Inverz $(D)doc2.2 > $(D)inhalt2.1
	 sed -f $(B)ibf $(D)inhalt2.1 > $(D)inhalt2.2
	 nroff -ms $(D)inhalt2.2 > $(M)inhalt2
	 rm $(D)inhalt2.?
	 split -2160 $(D)doc2.2 $(D)doc2_
	 mv $(D)doc2_a? $(M)
	 rm $(D)doc2.1 $(D)doc2.2

doc2.lp:	$(OBJS)
		cat $(OBJS) > $(D)all2
		fgrep -v -f $(B)bdat $(D)all2 > $(D)all2.1
		sed -f $(B)bef1 $(D)all2.1 > $(D)doc2.2
		nroff $T -ms $(D)doc2.2 > $(D)doc2.3
		sed -f $(B)bfd $(D)doc2.3 > $(D)doc2.4
		split -2160 $(D)doc2.4 $(D)doc2.4_
		mv $(D)doc2.4_a? $(S)
		rm $(D)all2 $(D)all2.1 $(D)doc2.2 $(D)doc2.3 $(D)doc2.4

print:	$(M)doc2_aa
	lpr $(M)doc2_a?

#  zu doc2.lp
#
#  - fgrep filtert die Kommandos ".ft B" und ".ft R"
#    (nroff - Fettdruck) aus.
#  - sed ersetzt vor der Uebersetzung alle Umlaute und
#    nach der Uebersetzung stellt es vor jede Zeile
#    8 Leerzeichen und loescht die Unterstreichungen.
#
#########################################################
