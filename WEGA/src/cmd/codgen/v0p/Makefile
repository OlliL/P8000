# codgen v0p: the code generator of the old C compiler v0p (the compiler of the passes cparse, codgen, popt: v0 of 1982 with the differences of the passes), a native WEGA
# program. Made from the sources of .. with the switches of this version; start-up file and libc part are those of
# .. (make .. first).
#
#   make             v0p/codgen
#   make install PREFIX=dir    dir/lib/v0p/codgen (the pass of ../../cc/v0p/ccv0p)
#   make clean
#
# Switches: ../oldcc.h (the version name only: the native cc takes about 160 characters of -D options).

CC = cc
CFLAGS = -O
SWITCHES = -DOLD_V0P
PREFIX = /
LD = ld
OLD = ../../../lib/oldlibc
CRT = ../../../lib/libc/crt
OLDOBJS = $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o $(OLD)/flsbuf/0_2.o $(OLD)/filbuf/0_3.o $(OLD)/rdwr/0_3.o $(OLD)/fseek/0_2.o $(OLD)/ftell/0_2.o $(OLD)/cnvt/0_2.o $(OLD)/length/0_2.o $(OLD)/strcat/0_2.o $(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o $(OLD)/fopen/0_1.o $(OLD)/findiop/0_2.o $(OLD)/endopen/0_4.o $(OLD)/ungetc/0_1.o $(OLD)/malloc/0_5.o $(OLD)/setbuf/0_1.o $(OLD)/getw/0_1.o $(OLD)/isatty/0_2.o $(OLD)/freopen/0_1.o $(OLD)/stty/0_2.o
SHIPOBJS = ../__callex.o ../data.o ../brk.o ../cuexit.o ../ffltioc.o ../write.o ../close.o ../read.o ../lseek.o ../decimal.o ../cerror.o ../open.o ../creat.o ../ioctl.o ../umask.o ../chmod.o
OBJS = allo3.o local3.o match3.o order3.o out3.o regallo3.o regfile3.o tmp3.o zzz3.o reader3.o gen3.o optim3.o comm3.o rdr3.o main3.o efcode3.o lcl3.o tr3.o regs3.o table3_codgen.o ops_codgen.o hop_codgen.o

all: codgen

codgen: $(OBJS)
	$(LD) -X -e start -i $(CRT)/crt0-codgen.o $(OBJS) $(OLDOBJS) $(SHIPOBJS) -o codgen

allo3.o: ../allo3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../allo3.c

local3.o: ../local3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../local3.c

match3.o: ../match3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../match3.c

order3.o: ../order3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../order3.c

out3.o: ../out3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../out3.c

regallo3.o: ../regallo3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regallo3.c

regfile3.o: ../regfile3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regfile3.c

tmp3.o: ../tmp3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../tmp3.c

zzz3.o: ../zzz3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../zzz3.c

reader3.o: ../reader3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../reader3.c

gen3.o: ../gen3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../gen3.c

optim3.o: ../optim3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../optim3.c

comm3.o: ../comm3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../comm3.c

rdr3.o: ../rdr3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../rdr3.c

main3.o: ../main3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../main3.c

efcode3.o: ../efcode3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../efcode3.c

lcl3.o: ../lcl3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../lcl3.c

tr3.o: ../tr3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../tr3.c

regs3.o: ../regs3.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../regs3.c

table3_codgen.o: ../table3_codgen.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../table3_codgen.c

ops_codgen.o: ../ops_codgen.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../ops_codgen.c

hop_codgen.o: ../hop_codgen.c ../cg.h ../oldcc.h
	$(CC) $(CFLAGS) $(SWITCHES) -c ../hop_codgen.c

install: codgen
	-mkdir $(PREFIX)/lib/v0p
	cp codgen $(PREFIX)/lib/v0p/codgen

clean:
	rm -f codgen $(OBJS)
