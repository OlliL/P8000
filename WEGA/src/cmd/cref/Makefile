# cref: the cross reference of System III (usr/src/cmd/cref). So far the three tables of /usr/lib/cref that the WEGA root has, made the way
# System III makes them: ctab by mtab from ctable, cign by mign from csym, eign is a copy of esym (System III makes atab, etab and aign
# as well, and links /usr/lib/eign: the WEGA root has neither). mtab and mign are programs of the WEGA system that make runs itself
# (RUN: "wrun" under the emulator, empty on WEGA). The programs cref, crpost and upost are not here yet.
#
#   make install PREFIX=dir    install the tables below dir/usr/lib/cref

CC = cc
PREFIX = /

all: ctab cign eign

mtab: mtab.c econs.h
	$(CC) mtab.c -o mtab
mign: mign.c mcons.h
	$(CC) mign.c -o mign

ctab: mtab ctable
	$(RUN) ./mtab ctable ctab
cign: mign csym
	$(RUN) ./mign csym cign
eign: esym
	cp esym eign

install: all
	mkdir -p $(PREFIX)/usr/lib/cref
	cp cign $(PREFIX)/usr/lib/cref/cign ; chmod 664 $(PREFIX)/usr/lib/cref/cign
	cp ctab $(PREFIX)/usr/lib/cref/ctab ; chmod 664 $(PREFIX)/usr/lib/cref/ctab
	cp eign $(PREFIX)/usr/lib/cref/eign ; chmod 644 $(PREFIX)/usr/lib/cref/eign

clean:
	rm -f mtab mign ctab cign eign

.PHONY: all install clean
