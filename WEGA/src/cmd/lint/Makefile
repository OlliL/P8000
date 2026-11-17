# lint: the libraries of lint (usr/lib/llib-*) as they are in the WEGA root. System III makes llib-lc.ln and llib-port.ln with lint1 over
# the llib-lc and llib-port (cpp -C llib-lc | lint1 -vx >llib-lc.ln), but that does not give these files: the same call with the lint1,
# the llib-lc and llib-port of the WEGA root gives other bytes (in the emulator and on the real system: llib-port.ln has the flags "used in
# a value / effects context" set for eleven functions of stdio, llib-lc.ln other line numbers): the files were made from other inputs.
# So the .ln files are copied. The programs lint1, lint2 and lint (the script) are not here yet.
#
#   make install PREFIX=dir    install below dir/usr/lib

PREFIX = /
LIBS = llib-lc llib-lc.ln llib-lm llib-port llib-port.ln

all:

install:
	mkdir -p $(PREFIX)/usr/lib
	for f in $(LIBS) ; do cp $$f $(PREFIX)/usr/lib/$$f ; chmod 644 $(PREFIX)/usr/lib/$$f ; done

clean:

.PHONY: all install clean
