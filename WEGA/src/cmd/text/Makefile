# text: the nroff/troff support files of System III (usr/src/cmd/text): the macro packages (macros.d) and the terminal driving
# tables (roff.d/terms.d). The files are the ones of the WEGA root, except where System III makes them from sources (the tables).
#
#   make install PREFIX=dir    install below dir/usr/lib

PREFIX = /
SUBDIRS = macros.d roff.d shells.d

all:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) all ; cd .. ; done

install:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all install clean
