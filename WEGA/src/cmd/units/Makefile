# units: the table of the units program (/usr/lib/units). System III and Plexus have it as unittab.sh (Plexus' unittab.rc: cp unittab.sh unittab;
# install -f /usr/lib unittab; units.c reads /usr/lib/unittab); the WEGA root has it as /usr/lib/units, with other lines than the ones of
# System III and Plexus (the file here is that of the WEGA root). The program units is not here yet.
#
#   make install PREFIX=dir    install dir/usr/lib/units (mode 664)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib
	cp unittab.sh $(PREFIX)/usr/lib/units ; chmod 664 $(PREFIX)/usr/lib/units

clean:

.PHONY: all install clean
