# ed: /bin/ed from ed.c of System III (the WEGA sccsid, ed.c 1.2 01/30/83), ccv1 -DPWB, libc-v4; NOT LINKED YET, not in SUBDIRS
# (compare/ed/findings.json). ed.mk and edfun are the System III build files (not used).
#
#   make              build ./ed
#   make install PREFIX=dir
#   make clean

CC = ccv1
CAS = cas
LD = ld
CFLAGS = -O -I../../head/oldv7 -DPWB
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v4.a
CRT = ../../lib/libc/nonseg/crt

all: ed

ed: crt0.o ed.o $(LIBC)
	$(LD) -e start -o $@ crt0.o ed.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ed > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

ed.o: ed.c
	$(CC) $(CFLAGS) -c ed.c

install: ed
	cp ed $(PREFIX)/bin/ed
	$(STRIP) $(PREFIX)/bin/ed

clean:
	rm -f *.o crt0.s ed

.PHONY: all install clean
