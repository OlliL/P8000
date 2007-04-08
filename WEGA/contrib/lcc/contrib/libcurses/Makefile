#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1 (Berkeley) 6/7/85
#
# curses package
#

LIB=		Llibcurses.a
PREFIX=		/z/LCC
CC=		lcc

#LIB=		libcurses.a
#PREFIX=	/usr
#CC=		cc

#LIB=		slibcurses.a
#PREFIX=	/usr
#CC=		scc

CFLAGS=		-O

AR=		-ar
ARFLAGS=	rvu

CHMOG=		chmog
CP=		cp
RANLIB=		ranlib
LIBMODE=	644
LIBOWNER=	wega
LIBGROUP=	system

CFILES=	addch.c addstr.c box.c clear.c clrtobot.c clrtoeol.c cr_put.c \
	cr_tty.c curses.c delch.c deleteln.c delwin.c endwin.c erase.c \
	getch.c getstr.c idlok.c id_subwins.c initscr.c insch.c \
	insertln.c move.c mvprintw.c mvscanw.c mvwin.c newwin.c \
	overlay.c overwrite.c printw.c putchar.c refresh.c scanw.c scroll.c \
	toucholap.c standout.c touchwin.c tstp.c unctrl.c
#CFILES+=fullname.c longname.c

OBJS=	addch.o addstr.o box.o clear.o clrtobot.o clrtoeol.o cr_put.o \
	cr_tty.o curses.o delch.o deleteln.o delwin.o endwin.o erase.o \
	getch.o getstr.o idlok.o id_subwins.o initscr.o insch.o \
	insertln.o move.o mvprintw.o mvscanw.o mvwin.o newwin.o \
	overlay.o overwrite.o printw.o putchar.o refresh.o scanw.o scroll.o \
	toucholap.o standout.o touchwin.o tstp.o unctrl.o
#OBJS+=	fullname.o longname.o

all:	$(LIB)

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	
install:	all
	$(CP) $(LIB) $(PREFIX)/lib
	$(CHMOG) $(LIBMODE) $(LIBOWNER) $(LIBGROUP) $(PREFIX)/lib/$(LIB)
	$(RANLIB) $(PREFIX)/lib/$(LIB)

clean:
	-rm -f *.o $(LIB)

test:	$(LIB) test.o
	${CC} ${LDFLAGS} ${CFLAGS} -o test test.o $(LIB) -ltermlib   
        
test.o:	test.c
	${CC} ${CFLAGS} -c test.c
