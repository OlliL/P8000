OBJ=		sa.timer.o u130.o rtc72421.o gmtime.o timegm.o
M=		../common
CC=		cc
LD=		ld
CFLAGS= 	-O -I$(M)
CC_CMD= 	$(CC) -c $(CFLAGS)

all:	sa.timer

# files are here: src/uts/conf/
libb.a:	bmisc.o sa.function.o
	rm -f libb.a
	ar qc libb.a sa.function.o bmisc.o

u130.o:
	$(CC_CMD) $(M)/u130.c

rtc72421.o:
	$(CC_CMD) $(M)/rtc72421.c

gmtime.o:
	$(CC_CMD) $(M)/gmtime.c

timegm.o:
	$(CC_CMD) $(M)/timegm.c

sa.timer:	$(OBJ) libb.a
	$(LD) -s -o $@ $(OBJ) libb.a
	chmod 400 $@

install:	sa.timer
	@echo "A backup of the old sa.timer can be found here: /sa.timer.orig"
	sh -c 'if test ! -f /sa.timer.orig ; then \
		cp /sa.timer /sa.timer.orig ; \
	fi'
	cp sa.timer /sa.timer

clean:	
	rm -f $(OBJ) libb.a sa.timer bmisc.o sa.function.o
