# Bemerkungen:
#
#    -Es besteht die Moeglichkeit, mit dem Makefile die
#     Dokumentation von ING_DB fuer verschiedene Drucker
#     herzustellen.
#
#     BITTE VERAENDERN SIE KEINE DATEINAMEN !!!
#     Am besten ist, sie lassen die Directorys
#     'ing_doc' und 'ing_src' so wie sie sind.
#
#    -In der Titelfestlegung kann bei 'T = ' noch eine
#     Option fuer 'nroff' gegeben werden, die speziell
#     fuer besondere Drucker aufbereitet (Je nach den 
#     'nroff'- Moeglichkeiten des Rechners).
#
#    -'doc1' schafft den ersten Teil der Dokumentation kom-
#     plett nach /z/ing_doc/teil1/doc1. Der Text ist nicht
#     speziell aufbereitet.
#
#    -'doc1.tx' bereitet den Text fuer normale Nadeldrucker
#     auf. Es sind Umlaute vorgesehen und "nroff" fuehrt
#     Fettdruck aus.
#     Es sind keine "tab's" gesetzt.
#     Der Teil1 des Handbuchs wird anschliessend in Bloecke zu
#     30 Seiten geteilt, im directory /z/ing_doc/teil1 abgelegt
#     und danach gedruckt.
#     EINGABE: make doc1.tx
#
#    -'doc1.lp bereitet das Handbuch fuer Seriendrucker auf.
#     Es werden kein Fettdruck und keine Unterstreichungen
#     ausgefuehrt.
#     Die Dokumentation wird im directory /z/ing_src/lp
#     abgelegt.
#     Ansonsten gilt dasselbe wie bei 'doc1.tx'.
#     EINGABE: make doc1.lp
#
#############################################################


M = /z/ing_doc/teil1/
S = /z/ing_src/teil1/lp/
D = /z/ing_src/teil1/
B = /z/ing_src/teil1/befehle/
C = /z/ing_doc/c/
T =

OBJS =	$(D)deck $(D)einlei $(D)query $(D)monitor $(D)makro	\
	$(D)ql $(D)unix1.1 $(D)unix2 $(D)report $(D)files	\
	$(D)error

doc1:	$(OBJS)
	nroff -ms $(OBJS) > $(M)doc1

doc1.tx: $(OBJS)
	 nroff -ms $(OBJS) > $(D)doc1.1
	 $(D)Inverz $(D)doc1.1 > $(D)inhalt1.1
	 sed -f $(B)ibf $(D)inhalt1.1 > $(D)inhalt1.2
	 nroff -ms $(D)inhalt1.2 > $(M)inhalt1
	 rm $(D)inhalt1.?
	 split -2160 $(D)doc1.1 $(D)doc1_
	 mv $(D)doc1_a? $(M)
	 rm $(D)doc1.1

doc1.lp:	$(OBJS)
		cat $(OBJS) > $(D)all1
		fgrep -v -f $(B)bdat $(D)all1 > $(D)all1.1
		sed -f $(B)bef1 $(D)all1.1 > $(D)doc1.2
		nroff $T -ms $(D)doc1.2 > $(D)doc1.3
		sed -f $(B)bfd $(D)doc1.3 > $(D)doc1.4
		split -2160 $(D)doc1.4 $(D)doc1.4_
		mv $(D)doc1.4_a? $(S)
		rm $(D)all1 $(D)all1.1 $(D)doc1.2 $(D)doc1.3 $(D)doc1.4

print:	$(M)doc1_aa
	lpr $(M)doc1_a?

#  zu doc1.lp:
#
#  - fgrep filtert die Kommandos ".ft B" und ".ft R"
#    (nroff - Fettdruck) aus.
#  - sed ersetzt vor der Uebersetzung alle Umlaute und
#    nach der Uebersetzung stellt es vor jede Zeile
#    8 Leerzeichen und loescht die Unterstreichungen.
#
#########################################################
