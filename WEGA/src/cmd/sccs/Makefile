# sccs: the SCCS commands of System III (usr/src/cmd/sccs). Here only the help files of the command help (help.d) so far.
PREFIX = /
SUBDIRS = help.d

all:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) all ; cd .. ; done

install:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) install PREFIX=$(PREFIX) ; cd .. ; done

clean:
	for d in $(SUBDIRS) ; do cd $$d ; $(MAKE) clean ; cd .. ; done

.PHONY: all install clean
