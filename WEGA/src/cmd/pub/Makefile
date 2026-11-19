# pub: the files of /usr/pub that Plexus' pub.rc installs (cp ascii.pub ascii; install -f /usr/pub ascii; the same for eqnchar and greek).
# The files here are named after their installed names; 4.4BSD-Lite's share/misc copies ascii and eqnchar in the same way. dmalias and termlist have no
# source in System III, Plexus or 4.4BSD: they are the files of the WEGA root.
#
#   make install PREFIX=dir    install below dir/usr/pub (mode 644)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/pub
	cp ascii $(PREFIX)/usr/pub/ascii
	chmod 644 $(PREFIX)/usr/pub/ascii
	cp eqnchar $(PREFIX)/usr/pub/eqnchar
	chmod 644 $(PREFIX)/usr/pub/eqnchar
	cp greek $(PREFIX)/usr/pub/greek
	chmod 644 $(PREFIX)/usr/pub/greek
	cp dmalias $(PREFIX)/usr/pub/dmalias
	chmod 644 $(PREFIX)/usr/pub/dmalias
	cp termlist $(PREFIX)/usr/pub/termlist
	chmod 644 $(PREFIX)/usr/pub/termlist

clean:

.PHONY: all install clean
