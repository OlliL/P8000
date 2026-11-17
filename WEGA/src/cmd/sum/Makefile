# sum: byte-identical with /bin/sum (ccv0).
#
#   make              build ./sum
#   make install PREFIX=dir
#   make clean

CC = ccv0
CAS = cas
LD = ld
CFLAGS = -O -I../../head/oldv7
PREFIX = /
STRIP = strip
LIBC = ../../lib/libc-v1.a
CRT = ../../lib/libc/nonseg/crt

all: sum

sum: crt0.o sum.o $(LIBC)
	$(LD) -e start -o $@ crt0.o sum.o $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh sum > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

sum.o: sum.c
	$(CC) $(CFLAGS) -c sum.c

install: sum
	cp sum $(PREFIX)/bin/sum
	$(STRIP) $(PREFIX)/bin/sum

clean:
	rm -f *.o crt0.s sum

.PHONY: all install clean

# the library of /lib is not made here (make shipped)
/lib/libc.a /lib/slibc.a:
