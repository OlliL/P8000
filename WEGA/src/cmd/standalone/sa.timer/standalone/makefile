OBJ=		sa.timer.o u130.o rtc72421.o gmtime.o timegm.o
M=		../common
CC=		cc
LD=		ld
INCLUDES=	-I$(M)
CFLAGS= 	-O
CC_CMD= 	$(CC) -c $(CFLAGS) $(INCLUDES)

all:	sa.timer

u130.o:
	$(CC_CMD) $(M)/$*.c

rtc72421.o:
	$(CC_CMD) $(M)/$*.c

gmtime.o:
	$(CC_CMD) $(M)/$*.c

timegm.o:
	$(CC_CMD) $(M)/$*.c

sa.timer:	$(OBJ) libb.a
	$(LD) -s -o $@ $(OBJ) libb.a
	chmod 400 $@

install:	sa.timer
	@echo "A backup of the old sa.timer can be found here: /sa.timer.orig"
	if test ! -f /sa.timer.orig ; \
		cp /sa.timer /sa.timer.orig ; \
	fi
	cp sa.timer /sa.timer

clean:	
	rm -f $(OBJ) sa.timer
