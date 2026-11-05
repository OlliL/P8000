# rmdir: byte-identical with the original.
#
#   make              build ./rmdir
#   make install PREFIX=dir
#   make clean

CC = ccv1r
CAS = cas
LD = ld
CFLAGS = -O -I../cparse
PREFIX = /
LIBC = ../../lib/libc-v10.a
CRT = ../../lib/libc/crt

all: rmdir

rmdir: crt0.o rmdir.o $(LIBC)
	$(LD) -s -e start -o $@ crt0.o rmdir.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh rmdir > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

rmdir.o: rmdir.c
	$(CC) $(CFLAGS) -c rmdir.c

install: rmdir
	cp rmdir $(PREFIX)/bin/rmdir

clean:
	rm -f *.o crt0.s rmdir

.PHONY: all install clean
