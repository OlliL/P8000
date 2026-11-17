# pub: the files of /usr/pub that Plexus' pub.rc installs (cp ascii.pub ascii; install -f /usr/pub ascii; the same for eqnchar and greek);
# the .pub files are the files of the WEGA root (4.4BSD-Lite's share/misc copies ascii and eqnchar in the same way). dmalias and termlist have no
# source in System III, Plexus or 4.4BSD: they are the files of the WEGA root, kept as .pub in the same way.
#
#   make install PREFIX=dir    install below dir/usr/pub (mode 644)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/pub
	for f in ascii eqnchar greek dmalias termlist ; do cp $$f.pub $(PREFIX)/usr/pub/$$f ; chmod 644 $(PREFIX)/usr/pub/$$f ; done

clean:

.PHONY: all install clean
