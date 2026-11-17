# shells.d: the help files term and text of the text subsystem (/usr/lib/help), as they are in the WEGA root (mode 444). The shell scripts
# of System III's shells.d (mm, mmt, man, ...) are not here yet: they go to /usr/bin.
#
#   make install PREFIX=dir    copy them to dir/usr/lib/help

PREFIX = /
HELP = term text

all:

install:
	mkdir -p $(PREFIX)/usr/lib/help
	for f in $(HELP) ; do rm -f $(PREFIX)/usr/lib/help/$$f ; cp $$f $(PREFIX)/usr/lib/help/$$f ; chmod 444 $(PREFIX)/usr/lib/help/$$f ; done

clean:

.PHONY: all install clean
