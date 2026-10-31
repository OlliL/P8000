# ccv0: the driver of the old C compiler v0 (1982; host/OLDCC.md), a native WEGA program.
# It is ../cc.c compiled with -DOLD_V0: it runs the passes /lib/v0/cparse, /lib/v0/codgen and /lib/v0/popt (made
# in ../../cparse/v0, ../../codgen/v0, ../../popt/v0); cpp, gopt, cas, ld, crt0.o and libc.a are the shipped ones.
# The libc part is the one of .. (make .. first).
#
#   make                   v0/ccv0
#   make install PREFIX=dir    dir/bin/ccv0 (the passes: make install in their v0 directories)
#   make clean

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V0
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

all: ccv0

ccv0: cc.o $(OBJS)
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(OBJS)

cc.o: ../cc.c
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cc.c

install: ccv0
	cp ccv0 $(PREFIX)/bin/ccv0

clean:
	rm -f ccv0 cc.o
