# uts/dev/wega: the versions of lp.c, siok.c and cpu2.c that are linked into the shipped wega. The LIB2 of the same release
# (../LIB2) contains other versions of these members; LIB2W is that LIB2 with lp.o, siok.o and cpu2.o replaced.
# event.s is the version of conf/event.s linked into wega (the stub SIOXINT goes to GO_INT); the object must be named
# event.o, the name is in the symbol table of wega. conf links wega with LIB2W and this event.o.
#
#   make                       build LIB2W and event.o
#   make clean                 remove the objects and LIB2W

CC = scc
CFLAGS = -O
AS = as
AR = ar

all: LIB2W event.o

LIB2W: ../LIB2 lp.o siok.o cpu2.o
	cp ../LIB2 LIB2W
	$(AR) r LIB2W lp.o siok.o cpu2.o

event.o: event.s
	$(AS) -u -o $@ event.s

../LIB2:
	cd .. ; $(MAKE) LIB2

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f lp.o siok.o cpu2.o event.o LIB2W

install:

.PHONY: all clean install
