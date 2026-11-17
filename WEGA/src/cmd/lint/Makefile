# lint: the libraries of lint (usr/lib/llib-*). llib-lc, llib-lm and llib-port are the files of the WEGA root. llib-lc.ln and llib-port.ln
# are made the way System III's lint.mk makes them: cpp -C llib-lc | lint1 -vx >llib-lc.ln, cpp -C llib-port | lint1 -vxp >llib-port.ln
# (cpp and lint1 are those of the WEGA root, run by make: RUN is "wrun" under the emulator, empty on WEGA; they are not built here yet).
#
# The .ln files hold the line numbers of the declarations in the headers they include, and a number for every struct, and the headers of the
# WEGA root were reformatted after the libraries were made (a blank line after the comment on the first line, blank lines and a line of its
# own for the { of a struct). Found by trying the layouts until the files came out byte-identical (the layouts are made from ../../head here,
# in include/):
#   blank second line taken out:                grp.h nlist.h pwd.h sgtty.h s.out.h stdio.h sys/lockblk.h sys/stat.h time.h
#   all blank lines out and { joined to its line: sys/timeb.h sys/utsname.h
#   first line (the comment) out:               sys/times.h
# llib-lc of the root has the comment and /*LINTLIBRARY*/ in one line, the library was made with two lines (the line numbers).
#
#   make install PREFIX=dir    install below dir/usr/lib

PREFIX = /
HEAD = ../../head
LIBS = llib-lc llib-lc.ln llib-lm llib-port llib-port.ln
B2 = grp.h nlist.h pwd.h sgtty.h s.out.h stdio.h sys/lockblk.h sys/stat.h time.h
NBJ = sys/timeb.h sys/utsname.h
all: llib-lc.ln llib-port.ln

# one rule with one target (a rule with several targets would run its commands once for each of them under make -j)
headers: $(HEAD)/stdio.h
	mkdir -p include/sys
	for f in $(B2) ; do sed '2{/^$$/d;}' $(HEAD)/$$f > include/$$f ; done
	for f in $(NBJ) ; do awk '/^$$/ {next} $$0 == "{" && have { printf "%s{\n", prev; have = 0; next } have { print prev } { prev = $$0; have = 1 } END { if (have) print prev }' $(HEAD)/$$f > include/$$f ; done
	sed '1d' $(HEAD)/sys/times.h > include/sys/times.h
	touch headers

llib-lc.ln: llib-lc headers
	mkdir -p ln
	( echo '/* 2.2. */' ; echo '	/*LINTLIBRARY*/' ; sed '1d' llib-lc ) > ln/llib-lc
	cd ln ; $(RUN) /lib/cpp -C -I../include llib-lc | $(RUN) /usr/lib/lint1 -vx -H../hlint > ../llib-lc.ln
	rm -f hlint

# after llib-lc.ln: both rules use and remove the file hlint of lint1 (in parallel one removes it while the other reads it)
llib-port.ln: llib-port headers llib-lc.ln
	mkdir -p ln
	cp llib-port ln/llib-port
	cd ln ; $(RUN) /lib/cpp -C -I../include llib-port | $(RUN) /usr/lib/lint1 -vxp -H../hlint > ../llib-port.ln
	rm -f hlint

install: all
	mkdir -p $(PREFIX)/usr/lib
	for f in $(LIBS) ; do cp $$f $(PREFIX)/usr/lib/$$f ; chmod 644 $(PREFIX)/usr/lib/$$f ; done

clean:
	rm -rf include ln hlint headers llib-lc.ln llib-port.ln

.PHONY: all install clean
