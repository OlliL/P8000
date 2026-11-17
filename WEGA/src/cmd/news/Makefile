# news: the items of /usr/lib/news (the files of the WEGA root; no source in System III, Plexus or 4.4BSD). The program news is not here yet.
#
#   make install PREFIX=dir    install below dir/usr/lib/news (mode 664)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/news
	for f in news wega ; do cp $$f $(PREFIX)/usr/lib/news/$$f ; chmod 664 $(PREFIX)/usr/lib/news/$$f ; done

clean:

.PHONY: all install clean
