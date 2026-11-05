# dc: byte-identical with /bin/dc (ccv0).
#
#   make              build ./dc
#   make install PREFIX=dir
#   make clean

CC = ccv0
CAS = cas
LD = ld
CFLAGS = -O -I../cparse
PREFIX = /
LIBC = ../../lib/libc-v1.a
CRT = ../../lib/libc/crt

all: dc

dc: crt0.o dc1.o dc2.o $(LIBC)
	$(LD) -s -e start -o $@ crt0.o dc1.o dc2.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh dc > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

dc1.o: dc1.c
	$(CC) $(CFLAGS) -c dc1.c

dc2.o: dc2.c
	$(CC) $(CFLAGS) -c dc2.c

dc1.o dc2.o: dc.h

install: dc
	cp dc $(PREFIX)/bin/dc

clean:
	rm -f *.o crt0.s dc

.PHONY: all install clean
