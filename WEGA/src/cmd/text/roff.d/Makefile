# roff.d: the sources of nroff and troff are not here yet (programs). Here: the terminal driving tables (terms.d) and suftab, the table of
# the suffixes for the hyphenation of nroff/troff (/usr/lib/suftab): suftabmake.c includes the tables of suftab.c and writes the file suftab
# (a program of the WEGA system, run by make: RUN is what runs it, "wrun" under the emulator, empty on WEGA itself).
#
#   make install PREFIX=dir    install below dir/usr/lib (mode 644)

CC = cc
PREFIX = /
SUBDIRS = terms.d

all: suftab
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) all ; cd .. ; done

suftab: suftabmake.c suftab.c
	$(CC) -o suftabmake suftabmake.c
	$(RUN) ./suftabmake
	rm -f suftabmake

install: all
	mkdir -p $(PREFIX)/usr/lib
	cp suftab $(PREFIX)/usr/lib/suftab ; chmod 644 $(PREFIX)/usr/lib/suftab
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	rm -f suftabmake suftab a.out
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all install clean
