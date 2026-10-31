# ccv1: the driver of the old C compiler v1 (the revision that built the WEGA binaries), a native WEGA program.
# It is ../cc.c compiled with -DOLD_V1: it runs the passes /lib/v1/cparse, /lib/v1/codgen and /lib/v1/popt (made
# in ../../cparse/v1, ../../codgen/v1, ../../popt/v1); cpp, gopt, cas, ld, crt0.o and libc.a are the shipped ones.
# The libc part is the one of .. (make .. first).
#
#   make                   v1/ccv1
#   make install PREFIX=dir    dir/bin/ccv1 (the passes: make install in their v1 directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V1
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

all: ccv1

ccv1: cc.o $(OBJS)
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(OBJS)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv1
	cp ccv1 $(PREFIX)/bin/ccv1

clean:
	rm -f ccv1 cc.o
