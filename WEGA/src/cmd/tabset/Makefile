# tabset: the tab settings of /usr/lib/tabset (read by tabs): 4.4BSD-Lite's share/tabset/Makefile copies them; the files are those of the WEGA root.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/tabset
	rm -f $(PREFIX)/usr/lib/tabset/std ; cp std $(PREFIX)/usr/lib/tabset/std ; chmod 644 $(PREFIX)/usr/lib/tabset/std
	rm -f $(PREFIX)/usr/lib/tabset/vt100 ; cp vt100 $(PREFIX)/usr/lib/tabset/vt100 ; chmod 644 $(PREFIX)/usr/lib/tabset/vt100

clean:

.PHONY: all install clean
