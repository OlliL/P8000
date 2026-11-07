# write: byte-identical with the original.
#
#   make              build ./write
#   make install PREFIX=dir
#   make clean

CC = ccv1r
CAS = cas
LD = ld
CFLAGS = -O -I../cparse
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v10.a
CRT = ../../lib/libc/crt

all: write

write: crt0.o write.o $(LIBC)
	$(LD) -e start -o $@ crt0.o write.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh write > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

write.o: write.c
	$(CC) $(CFLAGS) -c write.c

install: write
	cp write $(PREFIX)/bin/write
	$(STRIP) $(PREFIX)/bin/write

clean:
	rm -f *.o crt0.s write

.PHONY: all install clean
