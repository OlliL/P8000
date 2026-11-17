# me: the macros of the -me package of nroff (/usr/lib/me), made the way 4.4BSD-Lite's share/me/Makefile makes them: the sources NAME.me hold their
# comments, strip.sed takes the comments out (from the line %beginstrip% on) and puts the line "This version has had comments stripped" in front
# of them; revisions is copied. The file tmac.e (/usr/lib/tmac) is made the same way in BSD (tmac.orig_me): here it is still the WEGA file
# (../text/macros.d).
#
# The sources are those of 4.4BSD-Lite with the lines changed, added and taken out that make the stripped files those of the WEGA root (they
# are an older version of the package; 4.4BSD's differ); the lines in front of the marker are those that stay in the WEGA files, the BSD
# copyright is behind the marker (and so not in the stripped file). strip.sed is BSD's without the rule that deletes lines with \n@ (the WEGA
# files have them) and with a line that leaves lines of the form ". \" alone (a comment line of local.me stays in the WEGA file).
# revisions is the file of the WEGA root (BSD copies it as it is; its text is another).
#
#   make install PREFIX=dir    install below dir/usr/lib/me (mode 664)

PREFIX = /
SRCS = acm.me chars.me deltext.me eqn.me float.me footnote.me index.me local.me null.me sh.me tbl.me thesis.me

all: stripped

# one rule, one target (a rule with several targets runs its commands once for each of them under make -j)
stripped: strip.sed $(SRCS)
	mkdir -p strip
	for i in $(SRCS) ; do sed -f strip.sed < $$i > strip/$$i ; done
	touch stripped

install: all
	mkdir -p $(PREFIX)/usr/lib/me
	for i in $(SRCS) ; do cp strip/$$i $(PREFIX)/usr/lib/me/$$i ; chmod 664 $(PREFIX)/usr/lib/me/$$i ; done
	cp revisions $(PREFIX)/usr/lib/me/revisions ; chmod 664 $(PREFIX)/usr/lib/me/revisions

clean:
	rm -rf strip stripped

.PHONY: all install clean
