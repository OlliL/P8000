# mv: also cp and ln, byte-identical with the originals.
#
#   make              build ./mv
#   make install PREFIX=dir
#   make clean

CC = ccv1
CAS = cas
LD = ld
CFLAGS = -O -I../cparse
PREFIX = /
LIBC = ../../lib/libc-v6.a
CRT = ../../lib/libc/crt

all: mv

mv: crt0.o mv.o $(LIBC)
	$(LD) -s -e start -o $@ crt0.o mv.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh mv > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

mv.o: mv.c
	$(CC) $(CFLAGS) -c mv.c

install: mv
	cp mv $(PREFIX)/bin/mv

clean:
	rm -f *.o crt0.s mv

.PHONY: all install clean
