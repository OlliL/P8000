# roff.d: the sources of nroff and troff are not here yet (programs); the terminal driving tables are in terms.d.

PREFIX = /
SUBDIRS = terms.d

all:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) all ; cd .. ; done

install:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all install clean
