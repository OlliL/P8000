# acct: the table of holidays of the accounting (/usr/lib/acct/holidays): acct.mk of Plexus copies it (cp holidays.sh holidays; install); the file is the one of the WEGA root. The programs of acct are not here yet.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/acct
	rm -f $(PREFIX)/usr/lib/acct/holidays ; cp holidays $(PREFIX)/usr/lib/acct/holidays ; chmod 750 $(PREFIX)/usr/lib/acct/holidays

clean:

.PHONY: all install clean
