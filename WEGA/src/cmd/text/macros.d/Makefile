# macros.d: the nroff macro packages. /usr/lib/tmac (tmac.an, tmac.e, ...): copied as they are in the WEGA root (System III copies
# tmac.an, tmac.m and tmac.v from here too; tmac.e, tmac.s and the tmac.s* files have no System III source).
# /usr/lib/macros (mmn, mmt, vmca): made from NAME.src by macrunch, as in System III. The .src files of System III are another version
# than the packages of the WEGA root (checked: macrunch and ntar of System III under the emulator give other bytes and 100 to 4000
# other lines), and the WEGA files keep their comments and the NOCOMPACT part with its markers. So here the .src files are the packages
# of the WEGA root, and macrunch has the option -r (WEGA's addition) that takes the .src as it is: no sed, no ntar. (ntar.c and the
# compaction by nroff -k of System III are not used and not here.)
#
#   make install PREFIX=dir    copy the files to dir/usr/lib/tmac (mode 644) and dir/usr/lib/macros (mode 664)

PREFIX = /
MACROS = mmn mmt vmca
TMAC = tmac.an tmac.e tmac.m tmac.s tmac.scover tmac.sdisp tmac.skeep tmac.srefs tmac.v

all: $(MACROS)

mmn: mmn.src macrunch
	sh ./macrunch -r mmn
mmt: mmt.src macrunch
	sh ./macrunch -r mmt
vmca: vmca.src macrunch
	sh ./macrunch -r vmca

install: all
	mkdir -p $(PREFIX)/usr/lib/tmac $(PREFIX)/usr/lib/macros
	for f in $(TMAC) ; do cp $$f $(PREFIX)/usr/lib/tmac/$$f ; chmod 644 $(PREFIX)/usr/lib/tmac/$$f ; done
	for f in $(MACROS) ; do cp $$f $(PREFIX)/usr/lib/macros/$$f ; chmod 664 $(PREFIX)/usr/lib/macros/$$f ; done

clean:
	rm -f $(MACROS) NOCOMPACT*

.PHONY: all install clean
