# ranlib: byte-identical with /usr/bin/ranlib (ccv0).
#
#   make              build ./ranlib
#   make install PREFIX=dir
#   make clean

CC = ccv0
CAS = cas
LD = ld
CFLAGS = -O -I../cparse
PREFIX = /
LIBC = ../../lib/libc-v1.a
CRT = ../../lib/libc/crt

all: ranlib

ranlib: crt0.o ranlib.o $(LIBC)
	$(LD) -s -e start -o $@ crt0.o ranlib.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ranlib > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

ranlib.o: ranlib.c
	$(CC) $(CFLAGS) -c ranlib.c

install: ranlib
	cp ranlib $(PREFIX)/bin/ranlib

clean:
	rm -f *.o crt0.s ranlib

.PHONY: all install clean
