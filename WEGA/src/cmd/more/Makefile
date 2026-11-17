# more: the help text of more (/usr/lib/more.help): more.mk of Plexus (and 4.4BSD's Makefile) copy it; the file is the one of the WEGA root. The program more is not here yet.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib
	rm -f $(PREFIX)/usr/lib/more.help ; cp more.help $(PREFIX)/usr/lib/more.help ; chmod 444 $(PREFIX)/usr/lib/more.help

clean:

.PHONY: all install clean
