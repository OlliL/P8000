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
#   OLD_D20X  the loop inversion of a back jump before its cross-jump when the test is near the head and the
#             cross-jump would take one instruction at most (d20xinv, z21.c)
#   OLD_D20S  a conditional jump over a jump turned round before jumps to jumps are followed, when its label holds
#             a jump forwards (iterate, d20sfwd, z21.c)
#   OLD_D20L  a loop whose head stands behind a jump into the loop is inverted, also when the head block reaches
#             the back jump (codemove, z21.c)

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D20C -DOLD_D20X -DOLD_D20S -DOLD_D20L -DOLD_D20K -DOLD_D20B
PREFIX = /
LD = ld
OLD = ../../../lib/oldlibc
OLDOBJS = $(OLD)/freopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o $(OLD)/filbuf/0_3.o \
	$(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_4.o $(OLD)/cnvt/0_2.o \
	$(OLD)/length/0_2.o $(OLD)/malloc/0_3.o $(OLD)/isatty/0_2.o $(OLD)/stty/0_2.o \
	$(OLD)/strcat/0_2.o $(OLD)/strncmp/0_1.o $(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o \
	$(OLD)/atoi/0_1.o $(OLD)/strrchr/0_1.o
SHIPOBJS = ../__callex.o ../data.o ../brk.o ../cuexit.o ../ffltioc.o ../write.o ../close.o ../read.o \
	../lseek.o ../decimal.o ../cerror.o ../open.o ../creat.o ../ioctl.o ../umask.o ../chmod.o ../ctype_.o
# the libc part in the order of the original (the order of its libc.a)
LIBCOBJS = $(OLD)/freopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o ../ffltioc.o $(OLD)/filbuf/0_3.o \
	$(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_4.o ../data.o ../__callex.o ../cuexit.o \
	$(OLD)/atoi/0_1.o $(OLD)/cnvt/0_2.o ../decimal.o $(OLD)/length/0_2.o $(OLD)/malloc/0_3.o $(OLD)/isatty/0_2.o \
	$(OLD)/stty/0_2.o ../ctype_.o $(OLD)/strrchr/0_1.o $(OLD)/strcat/0_2.o $(OLD)/strncmp/0_1.o \
	$(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o ../brk.o ../chmod.o ../close.o ../creat.o ../ioctl.o ../lseek.o \
	../open.o ../read.o ../umask.o ../write.o ../cerror.o
OBJS = z20.o z21.o z22.o z23.o

all: popt

popt: $(OBJS) $(OLDOBJS) $(SHIPOBJS)
	$(LD) -X -e start -i ../crt0.o $(OBJS) $(LIBCOBJS) -o popt

z20.o: ../z20.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z20.c

z21.o: ../z21.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z21.c

z22.o: ../z22.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z22.c

z23.o: ../z23.c ../z2.h ../stdio.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../z23.c

install: popt
	-mkdir $(PREFIX)/lib/v0p
	cp popt $(PREFIX)/lib/v0p/popt

clean:
	rm -f popt $(OBJS)
