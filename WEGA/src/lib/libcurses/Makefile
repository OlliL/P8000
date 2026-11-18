# libcurses: /usr/lib/libcurses.a and /usr/lib/slibcurses.a (the screen library, nonsegmented and segmented). System III
# has none, the 2.11BSD one is the later 4.3BSD curses: from 4.2BSD (usr/src/usr.lib/libcurses, as in the unix-history-repo
# of the TUHS trees; WEGA's curses.h is the 4.2BSD one without unctrl), initscr.c and scroll.c from 4.1cBSD. Both libraries
# are made from the same sources (1988, no Library.SC) by the shipped compiler (cc, scc) without -O; curses.h from
# src/head. WEGA's forms: compare/libcurses/SYSIII.md. Headers: arhdr_libcurses.s, arhdr_slibcurses.s, mkar.sh.
#
#   make        libcurses.a slibcurses.a
#   make install PREFIX=dir   dir/usr/lib/libcurses.a, dir/usr/lib/slibcurses.a
#   make clean

CC = cc
SCC = scc
CAS = cas
PREFIX = /
CFLAGS = -I../../../../head
OBJS = box.o clear.o clrtobot.o delch.o deleteln.o endwin.o initscr.o cr_tty.o delwin.o insch.o insertln.o longname.o \
	mvprintw.o mvscanw.o mvwin.o newwin.o overlay.o overwrite.o printw.o addstr.o scanw.o getstr.o getch.o addch.o \
	clrtoeol.o move.o standout.o unctrl.o refresh.o cr_put.o erase.o scroll.o curses.o touchwin.o

all: build/nonseg/libcurses.a build/seg/slibcurses.a

build/nonseg/libcurses.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile libcurses.a

build/seg/slibcurses.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) -f ../../Makefile slibcurses.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libcurses.a $(PREFIX)/usr/lib/libcurses.a
	cp build/seg/slibcurses.a $(PREFIX)/usr/lib/slibcurses.a

clean:
	cd build/nonseg ; rm -f libcurses.a  arhdr.o $(OBJS)
	cd build/seg    ; rm -f slibcurses.a arhdr.o $(OBJS)

##################
# bin targets
##################

libcurses.a: $(OBJS) ../../arhdr_libcurses.s ../../mkar.sh
	$(CAS) -u -o arhdr.o ../../arhdr_libcurses.s
	sh ../../mkar.sh libcurses.a arhdr.o $(OBJS)

slibcurses.a: $(OBJS) ../../arhdr_slibcurses.s ../../mkar.sh
	$(CAS) -u -o arhdr.o ../../arhdr_slibcurses.s
	sh ../../mkar.sh slibcurses.a arhdr.o $(OBJS)

###################
# obj targets
###################

box.o: ../../box.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../box.c
clear.o: ../../clear.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../clear.c
clrtobot.o: ../../clrtobot.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../clrtobot.c
delch.o: ../../delch.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../delch.c
deleteln.o: ../../deleteln.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../deleteln.c
endwin.o: ../../endwin.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../endwin.c
initscr.o: ../../initscr.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../initscr.c
cr_tty.o: ../../cr_tty.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../cr_tty.c
delwin.o: ../../delwin.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../delwin.c
insch.o: ../../insch.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../insch.c
insertln.o: ../../insertln.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../insertln.c
longname.o: ../../longname.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../longname.c
mvprintw.o: ../../mvprintw.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../mvprintw.c
mvscanw.o: ../../mvscanw.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../mvscanw.c
mvwin.o: ../../mvwin.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../mvwin.c
newwin.o: ../../newwin.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../newwin.c
overlay.o: ../../overlay.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../overlay.c
overwrite.o: ../../overwrite.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../overwrite.c
printw.o: ../../printw.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../printw.c
addstr.o: ../../addstr.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../addstr.c
scanw.o: ../../scanw.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../scanw.c
getstr.o: ../../getstr.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../getstr.c
getch.o: ../../getch.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../getch.c
addch.o: ../../addch.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../addch.c
clrtoeol.o: ../../clrtoeol.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../clrtoeol.c
move.o: ../../move.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../move.c
standout.o: ../../standout.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../standout.c
unctrl.o: ../../unctrl.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../unctrl.c
refresh.o: ../../refresh.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../refresh.c
cr_put.o: ../../cr_put.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../cr_put.c
erase.o: ../../erase.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../erase.c
scroll.o: ../../scroll.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../scroll.c
curses.o: ../../curses.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../curses.c
touchwin.o: ../../touchwin.c ../../curses.ext
	$(CC) $(CFLAGS) -c ../../touchwin.c

FORCE:

.PHONY: all install clean
