OBJ=		timer.o rtc72421.o u130.o gmtime.o timegm.o
M=		../common
CC=		scc
CFLAGS= 	-O -I$(M)
CC_CMD= 	$(CC) -c $(CFLAGS)

all:		kernel

u130.o:
	$(CC_CMD) $(M)/u130.c

rtc72421.o:
	$(CC_CMD) $(M)/rtc72421.c

gmtime.o:
	$(CC_CMD) $(M)/gmtime.c

timegm.o:
	$(CC_CMD) $(M)/timegm.c

kernel: 	$(OBJ)
	touch kernel

install: 	kernel
	@echo "I will now include the new objects in the kernel library LIB1."
	@echo "A backup of the old LIB1 can be found here: /usr/sys/sys/LIB1.orig"
	sh -c 'if test ! -f /usr/sys/sys/LIB1.orig ; then \
		cp /usr/sys/sys/LIB1 /usr/sys/sys/LIB1.orig ; \
	fi'
	ar rv /usr/sys/sys/LIB1 $(OBJ)
	@echo "You should now go and build a new WEGA kernel to get the RTC up and running"

clean:	
	rm -f $(OBJ) kernel
