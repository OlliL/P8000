OBJ=	sa.timer.o libb.a u130.o rtc72421.o gmtime.o timegm.o
CC=	cc
LD=	ld
DEBUG=
CFLAGS= -O $(DEBUG)

all:	sa.timer
sa.timer:	$(OBJ)
		$(LD) -s -o $@ $(OBJ)
		chmod 400 $@
clean:	
	rm -f $(OBJ) sa.timer
