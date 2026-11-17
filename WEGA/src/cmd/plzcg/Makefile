# plzcg: the error messages of the PLZ code generator (/usr/lib/plzcg.errs), the file of the WEGA root (no source in System III, Plexus or
# 4.4BSD). The programs of the PLZ compiler are not here yet.
#
#   make install PREFIX=dir    install below dir/usr/lib (mode 644)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib
	cp plzcg.errs $(PREFIX)/usr/lib/plzcg.errs ; chmod 644 $(PREFIX)/usr/lib/plzcg.errs

clean:

.PHONY: all install clean
