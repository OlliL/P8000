# bc: the library of bc (/usr/lib/lib.b): copied by the build of Plexus (build/everything.sh: cp lib.b.sh lib.b; install -n /usr/lib lib.b); the file is the one of the WEGA root. The program bc is not here yet.
#
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)

PREFIX = /

all:

install:
	mkdir -p $(PREFIX)/usr/lib
	rm -f $(PREFIX)/usr/lib/lib.b ; cp lib.b $(PREFIX)/usr/lib/lib.b ; chmod 644 $(PREFIX)/usr/lib/lib.b

clean:

.PHONY: all install clean
