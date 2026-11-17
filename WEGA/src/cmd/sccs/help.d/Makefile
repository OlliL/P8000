# help.d: the help files of the SCCS command help (/usr/lib/help), as they are in the WEGA root (they have the what line of the WEGA
# release as their first line). The modes are those of the WEGA root. term and text are in ../../text/shells.d.
#
#   make install PREFIX=dir    copy them to dir/usr/lib/help

PREFIX = /
RW = ad bd cmds de default ge he
RO = cb cm co prs rc un ut

all:

install:
	mkdir -p $(PREFIX)/usr/lib/help
	for f in $(RW) ; do cp $$f $(PREFIX)/usr/lib/help/$$f ; chmod 644 $(PREFIX)/usr/lib/help/$$f ; done
	for f in $(RO) ; do rm -f $(PREFIX)/usr/lib/help/$$f ; cp $$f $(PREFIX)/usr/lib/help/$$f ; chmod 444 $(PREFIX)/usr/lib/help/$$f ; done

clean:

.PHONY: all install clean
