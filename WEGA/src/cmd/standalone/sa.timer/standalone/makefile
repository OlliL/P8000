OBJ=	sa.timer.o u130.o rtc72421.o gmtime.o timegm.o
CC=	cc
LD=	ld
DEBUG=
CFLAGS= -O $(DEBUG)

all:	sa.timer
sa.timer:	$(OBJ) libb.a
		$(LD) -s -o $@ $(OBJ) libb.a
		chmod 400 $@
clean:	
	rm -f $(OBJ) timer.o sa.timer
