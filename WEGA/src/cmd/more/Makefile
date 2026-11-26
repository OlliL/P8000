# more (link page): /bin/more, more.c 1.13 and regex.c 1.1 03/31/83: Plexus' more (Berkeley more 4.1) as WEGA changed
# it (built without -DSYS3: the sgtty branch with TIOCSETP; its own SCCS stamps; setret/longret for setjmp/longjmp; stderr
# reopened for reading; printf through xprintf; a fifo shows no percentage; execute by execl; the WEGA magic numbers);
# whoami.h of Plexus; ccv1 with the stdio.h of the time (oldv7), libc-v6 (owner 2026-11-26: crt0 and date). Byte-identical.
# The help text of more (/usr/lib/more.help): more.mk of Plexus copies it; the file is the one of the WEGA root.
#
#   make                       build ./more
#   make install PREFIX=dir    install more and its link page below dir/bin, more.help below dir/usr/lib (the modes of
#                              the WEGA root)

CC = ccv1
CFLAGS = -O -I. -I../../head/oldv7
CAS = cas
LD = ld
STRIP = strip
PREFIX = /
LIBC = ../../lib/libc-v6.a
TERMLIB = ../../lib/libtermlib/build/nonseg/libtermlib.a
CRT = ../../lib/libc/nonseg/crt

all: more

more: crt0.o more.o regex.o $(TERMLIB) $(LIBC)
	$(LD) -e start -o $@ crt0.o more.o regex.o $(TERMLIB) $(LIBC)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh more > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

more.o: more.c whoami.h
	$(CC) $(CFLAGS) -c more.c

regex.o: regex.c
	$(CC) $(CFLAGS) -c regex.c

install: more
	mkdir -p $(PREFIX)/bin $(PREFIX)/usr/lib
	rm -f $(PREFIX)/bin/more $(PREFIX)/bin/page
	cp more $(PREFIX)/bin/more ; $(STRIP) $(PREFIX)/bin/more ; ln $(PREFIX)/bin/more $(PREFIX)/bin/page
	rm -f $(PREFIX)/usr/lib/more.help ; cp more.help $(PREFIX)/usr/lib/more.help ; chmod 444 $(PREFIX)/usr/lib/more.help

clean:
	rm -f *.o crt0.s more

.PHONY: all install clean

/lib/libc.a /usr/lib/libtermlib.a:
