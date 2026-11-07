# rm: byte-identical with the original.
#
#   make              build ./rm
#   make install PREFIX=dir
#   make clean

CC = ccv1
CAS = cas
LD = ld
CFLAGS = -O -I../cparse
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v6.a
CRT = ../../lib/libc/crt

all: rm

rm: crt0.o rm.o $(LIBC)
	$(LD) -e start -o $@ crt0.o rm.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh rm > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

rm.o: rm.c
	$(CC) $(CFLAGS) -c rm.c

install: rm
	cp rm $(PREFIX)/bin/rm
	$(STRIP) $(PREFIX)/bin/rm

clean:
	rm -f *.o crt0.s rm

.PHONY: all install clean
