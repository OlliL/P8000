D = /z/ing_doc/
I = /z/ing_doc/inhalt/
C = /z/ing_doc/c/
B = /z/ing_doc/inhalt/befehle/

OBJS = $(I)Inverz

inh_all: $(OBJS)
	cat /z/ing_doc/teil1/doc* /z/ing_doc/teil2/doc*	\
	/z/ing_doc/teil3/doc* > $(I)all
	sed -f $(B)bfd $(I)all > $(I)all_ok
	rm $(I)all
	$(I)Inverz $(I)all_ok > $(I)inh_all
	rm $(I)all_ok
	sed -f $(B)bef $(I)inh_all > $(I)inh
	nroff -ms $(I)inh > $(I)Inhalt
	rm $(I)inh_all

print:	$(I)Inhalt
	lpr $(I)deck_inh $(I)Inhalt
