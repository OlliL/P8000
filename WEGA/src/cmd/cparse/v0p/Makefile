# cparse v0p: pass 1 the old C compiler v0p (the compiler of the passes cparse, codgen, popt: v0 of 1982 with the differences of the passes), a native WEGA program.
# Made from the sources of .. with the switches of this version; the start-up file, the bss layout and the libc
# part are those of .. (make .. first).
#
#   make             v0p/cparse
#   make install PREFIX=dir    dir/lib/v0p/cparse (the pass of ../../cc/v0p/ccv0p)
#   make clean
#
# Switches (host/OLDCC.md):
#   OLD_D12   the string literals of a file at the end of its data, one pool, aligned only at its start and end
#   OLD_D4R   no home in the frame for the register parameters behind the last other parameter (argalloc)
#   OLD_D6T   the values of a cpir switch go to codgen with its targets (record V), written together
#   OLD_D6C   up to 3 cases compared one after the other, the cpir table from 4 cases on

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_D12 -DOLD_D4R -DOLD_D6T -DOLD_D6C
PREFIX = /
LDBIN = /bin/ld
OLD = ../../../lib/oldlibc
HDRS = ../mfile1 ../manifest ../macdefs ../stdio.h
OBJS = ../bss.o cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o
LIBCOBJS = ../libcbss.o $(OLD)/fltioc/0_1.o $(OLD)/fseek/0_2.o $(OLD)/ftell/0_2.o $(OLD)/rdwr/0_3.o $(OLD)/fopen/0_1.o $(OLD)/fdopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o $(OLD)/ungetc/0_1.o ../shipped/atof.o $(OLD)/ecvt/0_1.o ../shipped/pow10.o $(OLD)/filbuf/0_3.o ../shipped/data.o $(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_4.o $(OLD)/findiop/0_2.o ../shipped/__callex.o ../shipped/cuexit.o $(OLD)/perror/0_1.o $(OLD)/cnvt/0_2.o ../shipped/decimal.o $(OLD)/length/0_3.o $(OLD)/malloc/0_3.o $(OLD)/errlst/0_1.o $(OLD)/isatty/0_2.o $(OLD)/stty/0_3.o $(OLD)/strcat/0_2.o $(OLD)/strcmp/0_1.o $(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o ../shipped/ctype_.o ../shipped/brk.o ../shipped/chmod.o ../shipped/close.o ../shipped/creat.o ../shipped/ioctl.o ../shipped/lseek.o ../shipped/open.o ../shipped/read.o ../shipped/signal.o ../shipped/umask.o ../shipped/write.o ../shipped/cerror.o

all: cparse

cparse: ../crt0.o $(OBJS) $(LIBCOBJS)
	$(LDBIN) -i -e start ../crt0.o $(OBJS) $(LIBCOBJS) -o $@

cgram1.o: ../cgram1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../cgram1.c

xdefs1.o: ../xdefs1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../xdefs1.c

scan1.o: ../scan1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../scan1.c

pftn1.o: ../pftn1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../pftn1.c

trees1.o: ../trees1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../trees1.c

optim1.o: ../optim1.c $(HDRS) ../optim
	$(CC) $(CFLAGS) $(SWITCHES) -c ../optim1.c

code1.o: ../code1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../code1.c

local1.o: ../local1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../local1.c

comm1.o: ../comm1.c $(HDRS) ../common
	$(CC) $(CFLAGS) $(SWITCHES) -c ../comm1.c

vinit1.o: ../vinit1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../vinit1.c

sym1.o: ../sym1.c $(HDRS)
	$(CC) $(CFLAGS) $(SWITCHES) -c ../sym1.c

install: cparse
	-mkdir $(PREFIX)/lib/v0p
	cp cparse $(PREFIX)/lib/v0p/cparse

clean:
	rm -f cparse *.o
