# strip: byte-identical with the original.
#
#   make              build ./strip
#   make install PREFIX=dir
#   make clean

CC = ccv1
CAS = cas
LD = ld
CFLAGS = -O
PREFIX = /
LIBC = ../../lib/libc-v4.a
CRT = ../../lib/libc/crt

all: strip

strip: crt0.o strip.o $(LIBC)
	$(LD) -s -e start -o $@ crt0.o strip.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh strip > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

strip.o: strip.c
	$(CC) $(CFLAGS) -c strip.c

install: strip
	cp strip $(PREFIX)/bin/strip

clean:
	rm -f *.o crt0.s strip

.PHONY: all install clean
