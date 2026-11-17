# pub: the files of /usr/pub: pub.rc of Plexus (cp ascii.pub ascii; install -f /usr/pub ascii, the same for eqnchar) and 4.4BSD-Lite's share/misc/Makefile copy them; the files are those of the WEGA root.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/pub
	rm -f $(PREFIX)/usr/pub/ascii ; cp ascii $(PREFIX)/usr/pub/ascii ; chmod 644 $(PREFIX)/usr/pub/ascii
	rm -f $(PREFIX)/usr/pub/eqnchar ; cp eqnchar $(PREFIX)/usr/pub/eqnchar ; chmod 644 $(PREFIX)/usr/pub/eqnchar

clean:

.PHONY: all install clean
