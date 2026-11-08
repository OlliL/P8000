# uts/h: the headers of the kernel configuration, installed to usr/sys/h.
#
#   make install PREFIX=dir    install below dir (default /)

PREFIX = /
IFILES = mdsize.h sysparm.h ver.h
IDIR = $(PREFIX)/usr/sys/h

all:

install:
	test -d $(PREFIX)/usr || mkdir $(PREFIX)/usr
	test -d $(PREFIX)/usr/sys || mkdir $(PREFIX)/usr/sys
	test -d $(IDIR) || mkdir $(IDIR)
	for f in $(IFILES); do cp $$f $(IDIR)/$$f; done

clean:

.PHONY: all install clean
