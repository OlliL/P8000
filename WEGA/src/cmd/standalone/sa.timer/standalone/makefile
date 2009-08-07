OBJ=	sa.timer.o u130.o rtc72421.o gmtime.o timegm.o libb.a
CC=	cc
LD=	ld
DEBUG=
CFLAGS= -O $(DEBUG)

all:	sa.timer
sa.timer.o:
		$(CC) -c $(DEBUG) sa.timer.c
sa.timer:	$(OBJ)
		$(LD) -s -o $@ $(OBJ)
		chmod 400 $@
