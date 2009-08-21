OBJ=		timer.o rtc72421.o u130.o gmtime.o timegm.o
M=		../common
CC=		scc
INCLUDES=	-I$(M)
CFLAGS= 	-O
CC_CMD= 	$(CC) -c $(CFLAGS) $(INCLUDES)

all:		kernel

u130.o:
	$(CC_CMD) $(M)/$*.c

rtc72421.o:
	$(CC_CMD) $(M)/$*.c

gmtime.o:
	$(CC_CMD) $(M)/$*.c

timegm.o:
	$(CC_CMD) $(M)/$*.c

kernel: 	$(OBJ)

install: 	kernel
	@echo "I will now include the new objects in the kernel library LIB1."
	@echo "A backup of the old LIB1 can be found here: /usr/sys/sys/LIB1.orig"
	if test ! -f /usr/sys/sys/LIB1.orig ; \
		cp /usr/sys/sys/LIB1 /usr/sys/sys/LIB1.orig ; \
	fi
	ar rv /usr/sys/sys/LIB1 timer.o rtc72421.o u130.o gmtime.o timegm.o
	@echo "You should now go and build a new WEGA kernel to get the RTC up and running!"

clean:	
	rm -f $(OBJ)
