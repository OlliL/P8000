OBJ=	sa.timer.o libb.a u130.o gmtime.o timegm.o
CC=	cc
LD=	ld
DEBUG=	
CFLAGS= -O $(DEBUG)

all:	sa.timer
sa.timer:	$(OBJ)
		$(LD) -s -o $@ $(OBJ)
		chmod 400 $@
