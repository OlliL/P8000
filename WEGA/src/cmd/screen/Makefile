# screen: the help texts of the full screen programs (/usr/lib/screen/vlshelp, vnewshelp): the files of the WEGA root (no source in System III, Plexus or 4.4BSD).
#
#   make install PREFIX=dir    install below dir/usr/lib/screen

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/screen
	cp vlshelp $(PREFIX)/usr/lib/screen/vlshelp ; chmod 644 $(PREFIX)/usr/lib/screen/vlshelp
	cp vnewshelp $(PREFIX)/usr/lib/screen/vnewshelp ; chmod 644 $(PREFIX)/usr/lib/screen/vnewshelp

clean:

.PHONY: all install clean
