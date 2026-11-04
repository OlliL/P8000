# size: byte-identical with the original.
#
#   make              build ./size
#   make install PREFIX=dir
#   make clean

CC = ccv1
CAS = cas
LD = ld
CFLAGS = -O
PREFIX = /
LIBC = ../../lib/libc-v3.a
CRT = ../../lib/libc/crt

all: size

size: crt0.o size.o $(LIBC)
	$(LD) -s -e start -o $@ crt0.o size.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh size > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

size.o: size.c
	$(CC) $(CFLAGS) -c size.c

install: size
	cp size $(PREFIX)/bin/size

clean:
	rm -f *.o crt0.s size

.PHONY: all install clean
