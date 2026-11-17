# acct: the shell scripts of the accounting (/usr/lib/acct, as in System III's acct.mk: cp NAME.sh NAME; install -f /usr/lib/acct NAME; mode 750) and the table of holidays of the accounting (/usr/lib/acct/holidays): acct.mk of Plexus copies it (cp holidays.sh holidays; install); the file is the one of the WEGA root. The programs of acct are not here yet.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib/acct
	for f in chargefee ckpacct dodisk lastlogin monacct nulladm prctmp prdaily prtacct remove runacct sdisk shutacct startup turnacct ; do rm -f $(PREFIX)/usr/lib/acct/$$f ; cp $$f.sh $(PREFIX)/usr/lib/acct/$$f ; chmod 750 $(PREFIX)/usr/lib/acct/$$f ; done
	mkdir -p $(PREFIX)/usr/lib/acct
	rm -f $(PREFIX)/usr/lib/acct/holidays ; cp holidays $(PREFIX)/usr/lib/acct/holidays ; chmod 750 $(PREFIX)/usr/lib/acct/holidays

clean:

.PHONY: all install clean
