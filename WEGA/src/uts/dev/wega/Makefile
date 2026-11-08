# uts/dev/wega: the versions of lp.c and siok.c that are linked into the shipped wega. The LIB2 of the same release
# (../LIB2) contains other versions of these two members; LIB2W is that LIB2 with lp.o and siok.o replaced.
# conf links wega with LIB2W.
#
#   make                       build LIB2W
#   make clean                 remove the objects and LIB2W

CC = scc
CFLAGS = -O
AR = ar

all: LIB2W

LIB2W: ../LIB2 lp.o siok.o
	cp ../LIB2 LIB2W
	$(AR) r LIB2W lp.o siok.o

../LIB2:
	cd .. ; $(MAKE) LIB2

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f lp.o siok.o LIB2W

install:

.PHONY: all clean install
