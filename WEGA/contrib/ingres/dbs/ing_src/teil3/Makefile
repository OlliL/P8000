M = /z/ing_doc/teil3/
S = /z/ing_src/teil3/lp/
D = /z/ing_src/teil3/
B = /z/ing_src/teil3/befehle/
C = /z/ing_doc/c/
T =

OBJS = $(D)amxzip $(D)report

doc3:	$(OBJS)
	nroff -ms $(OBJS) > $(M)doc3

doc3.tx: $(OBJS)
	 nroff -ms $(OBJS) > $(D)doc3.2
	 sed -f $(B)dbef $(D)doc3.2 > $(D)doc3.1
	 $(D)Inverz $(D)doc3.1 > $(D)inhalt3.1
	 sed -f $(B)ibf $(D)inhalt3.1 > $(D)inhalt3.2
	 nroff -ms $(D)inhalt3.2 > $(M)inhalt3
	 rm $(D)inhalt3.?
	 split -2160 $(D)doc3.1 $(D)doc3_
	 mv $(D)doc3_a? $(M)
	 rm $(D)doc3.1 $(D)doc3.2
	 nroff -ms $(D)deck > $(M)deck3

doc3.lp:	$(OBJS)
		cat $(OBJS) > $(D)all3
		fgrep -v -f bdat $(D)all3 > $(D)all3.1
		sed -f bef1 $(D)all3.1 > $(D)doc3.2
		nroff $T -ms $(D)doc3.2 > $(D)doc3.3
		sed -f bfd $(D)doc3.3 > $(D)doc3.4
		split -2160 $(D)doc3.4 $(D)doc3.4_
		mv $(D)doc3.4_a? $(S)
		rm $(D)all3 $(D)all3.1 $(D)doc3.2 $(D)doc3.3 $(D)doc3.4

print:	$(M)doc*
	lpr $(M) deck3 doc3_a?

