S = /z/ing_doc/sachwort/
Q = /z/ing_doc/sachwort/quell/
C = /z/ing_doc/c/

sawo:	$(Q)swverz.nq
	nroff -ms $(Q)swverz.nq > $(S)sachw
	nroff -ms $(Q)deck > $(S)deck_swv

print:	$(S)sachw 
	lpr $(S)deck_swv $(S)sachw

