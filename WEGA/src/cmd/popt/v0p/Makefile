# popt v0p: the peephole optimizer the old C compiler v0p (the compiler of the passes cparse, codgen, popt: v0 of 1982 with the differences of the passes), a native WEGA
# program. Made from the sources of .. with the switches of this version; start-up file and libc part are those of
# .. (make .. first).
#
#   make             v0p/popt
#   make install PREFIX=dir    dir/lib/v0p/popt (the pass of ../../cc/v0p/ccv0p)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D20C  a block reached only by conditional jumps (and by falling out of one over it) that ends with a jump
#             goes behind the conditional jump that names it from in front (cjmove, z21.c)

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D20C
PREFIX = /
LD = ld
OLD = ../../../lib/oldlibc
OLDOBJS = $(OLD)/freopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o $(OLD)/filbuf/0_3.o \
	$(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_4.o $(OLD)/cnvt/0_2.o \
	$(OLD)/length/0_2.o $(OLD)/malloc/0_3.o $(OLD)/isatty/0_2.o $(OLD)/stty/0_2.o \
	$(OLD)/strcat/0_2.o $(OLD)/strncmp/0_1.o $(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o \
	$(OLD)/atoi/0_1.o $(OLD)/strrchr/0_1.o
SHIPOBJS = ../__callex.o ../data.o ../brk.o ../cuexit.o ../ffltioc.o ../write.o ../close.o ../read.o \
	../lseek.o ../decimal.o ../cerror.o ../open.o ../creat.o ../ioctl.o ../umask.o ../chmod.o ../ctype_.o \
	../strchr.o ../strcmp.o
OBJS = z20.o z21.o z22.o z23.o

all: popt

popt: $(OBJS) $(OLDOBJS) $(SHIPOBJS)
	$(LD) -X -e start -i ../crt0.o $(OBJS) $(OLDOBJS) $(SHIPOBJS) -o popt

z20.o: ../z20.c ../z2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z20.c

z21.o: ../z21.c ../z2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z21.c

z22.o: ../z22.c ../z2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z22.c

z23.o: ../z23.c ../z2.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z23.c

install: popt
	-mkdir $(PREFIX)/lib/v0p
	cp popt $(PREFIX)/lib/v0p/popt

clean:
	rm -f popt $(OBJS)
