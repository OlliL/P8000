# bc: /bin/bc from bc.y (System III, the WEGA sccsid), as :mkcmd of System III: yacc bc.y, y.tab.c -> bc.c, ccv0;
# libc-v1.a (07/82). The library of bc (/usr/lib/lib.b): copied by the build of Plexus (build/everything.sh: cp lib.b.sh
# lib.b; install -n /usr/lib lib.b); the file is the one of the WEGA root.
#
#   make              build ./bc
#   make install PREFIX=dir    install below dir/ (the modes of the WEGA root)
#   make clean

CC = ccv0
CAS = cas
LD = ld
YACC = yacc
CFLAGS = -O -I../../head/oldv7
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v1.a
CRT = ../../lib/libc/nonseg/crt

all: bc

bc: crt0.o bc.o $(LIBC)
	$(LD) -e start -o $@ crt0.o bc.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh bc > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

bc.c: bc.y
	$(YACC) bc.y ; mv y.tab.c bc.c

bc.o: bc.c
	$(CC) $(CFLAGS) -c bc.c

install: bc
	mkdir -p $(PREFIX)/bin $(PREFIX)/usr/lib
	rm -f $(PREFIX)/bin/bc ; cp bc $(PREFIX)/bin/bc ; $(STRIP) $(PREFIX)/bin/bc ; chmod 751 $(PREFIX)/bin/bc
	rm -f $(PREFIX)/usr/lib/lib.b ; cp lib.b $(PREFIX)/usr/lib/lib.b ; chmod 644 $(PREFIX)/usr/lib/lib.b

clean:
	rm -f bc bc.c bc.o crt0.s crt0.o y.tab.c

.PHONY: all install clean

# the library of /lib is not made here (make shipped)
/lib/libc.a /lib/slibc.a:
