# problems: the files of /usr/lib/problems (scrb_date, seqno, strfile): the files of the WEGA root (no source in System III, Plexus or 4.4BSD).
#
#   make install PREFIX=dir    install below dir/usr/lib/problems

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/problems
	cp scrb_date $(PREFIX)/usr/lib/problems/scrb_date ; chmod 644 $(PREFIX)/usr/lib/problems/scrb_date
	cp seqno $(PREFIX)/usr/lib/problems/seqno ; chmod 666 $(PREFIX)/usr/lib/problems/seqno
	cp strfile $(PREFIX)/usr/lib/problems/strfile ; chmod 666 $(PREFIX)/usr/lib/problems/strfile

clean:

.PHONY: all install clean
