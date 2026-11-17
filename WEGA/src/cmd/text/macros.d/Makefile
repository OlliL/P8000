# macros.d: the nroff macro packages in /usr/lib/tmac, as they are in the WEGA root (System III copies tmac.an, tmac.m and tmac.v from
# here too; tmac.e, tmac.s and the tmac.s* files have no System III source). The packages of /usr/lib/macros (mmn, mmt, vmca) are made
# by macrunch from the .src files: not here yet.
#
#   make install PREFIX=dir    copy the files to dir/usr/lib/tmac (mode 644)

PREFIX = /
TMAC = tmac.an tmac.e tmac.m tmac.s tmac.scover tmac.sdisp tmac.skeep tmac.srefs tmac.v

all:

install:
	mkdir -p $(PREFIX)/usr/lib/tmac
	for f in $(TMAC) ; do cp $$f $(PREFIX)/usr/lib/tmac/$$f ; chmod 644 $(PREFIX)/usr/lib/tmac/$$f ; done

clean:

.PHONY: all install clean
