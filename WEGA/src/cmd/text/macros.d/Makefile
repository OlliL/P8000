# macros.d: the nroff macro packages. /usr/lib/tmac (tmac.an, tmac.e, ...): copied as they are in the WEGA root (System III copies
# tmac.an, tmac.m and tmac.v from here too; tmac.e, tmac.s and the tmac.s* files have no System III source).
# /usr/lib/macros (mmn, mmt, vmca): made from NAME.src by macrunch with the calls of System III's macros.mk (-n m mmn, -t m mmt, vmca).
# The .src files are those of System III with the lines changed that the packages of the WEGA root show differently (the names of the
# company and of the Tektronix scope taken out: 12, 14 and 6 lines); macrunch is changed as its head says: the packages of the WEGA root
# are the .src as they are (mmt, vmca) or without comment lines (mmn); nothing of what System III's macrunch does beyond that (sed, ntar,
# nroff -k) shows in them.
#
#   make install PREFIX=dir    copy the files to dir/usr/lib/tmac (mode 644) and dir/usr/lib/macros (mode 664)

PREFIX = /
MACROS = mmn mmt vmca
TMAC = tmac.an tmac.e tmac.m tmac.s tmac.scover tmac.sdisp tmac.skeep tmac.srefs tmac.v

all: $(MACROS)

mmn: mmn.src macrunch
	sh ./macrunch -n m mmn
mmt: mmt.src macrunch
	sh ./macrunch -t m mmt
vmca: vmca.src macrunch
	sh ./macrunch vmca

install: all
	mkdir -p $(PREFIX)/usr/lib/tmac $(PREFIX)/usr/lib/macros
	for f in $(TMAC) ; do cp $$f $(PREFIX)/usr/lib/tmac/$$f ; chmod 644 $(PREFIX)/usr/lib/tmac/$$f ; done
	for f in $(MACROS) ; do cp $$f $(PREFIX)/usr/lib/macros/$$f ; chmod 664 $(PREFIX)/usr/lib/macros/$$f ; done

clean:
	rm -f $(MACROS)

.PHONY: all install clean
