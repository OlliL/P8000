# ccv0p: the driver of the old C compiler v0p (the compiler of the passes; host/OLDCC.md), a native WEGA program.
# It is ../cc.c compiled with -DOLD_V0P: it runs the passes /lib/v0p/cparse, /lib/v0p/codgen and /lib/v0p/popt (made
# in ../../cparse/v0p, ../../codgen/v0p, ../../popt/v0p); cpp, gopt, cas, ld, crt0.o and libc.a are the shipped ones.
# The libc part is the one of .. (make .. first).
#
#   make                   v0p/ccv0p
#   make install PREFIX=dir    dir/bin/ccv0p (the passes: make install in their v0p directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V0P
LD = ld
PREFIX = /
OLD = ../../../lib/oldlibc
OBJS = ../freopen.o ../fopen.o $(OLD)/printf/0_4.o \
	$(OLD)/doprnt/0_4.o ../ffltioc.o ../flsbuf.o \
	../endopen.o ../findiop.o ../data.o \
	../__callex.o ../cuexit.o ../abort.o \
	$(OLD)/cnvt/0_1.o ../decimal.o $(OLD)/length/0_1.o \
	$(OLD)/malloc/0_4.o ../ctype_.o ../isatty.o \
	../stty.o $(OLD)/strcat/0_1.o ../strcmp.o \
	$(OLD)/strcpy/0_1.o $(OLD)/strlen/0_1.o ../access.o \
	../brk.o ../chmod.o ../close.o \
	../creat.o ../execl.o ../execv.o \
	../fork.o ../getpid.o ../ioctl.o \
	../lseek.o ../open.o ../signal.o \
	../stat.o ../umask.o ../unlink.o \
	../wait.o ../write.o ../cerror.o

all: ccv0p

ccv0p: cc.o $(OBJS)
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(OBJS)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv0p
	cp ccv0p $(PREFIX)/bin/ccv0p

clean:
	rm -f ccv0p cc.o
