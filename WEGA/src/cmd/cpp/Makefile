# cpp: the C preprocessor of WEGA (/lib/cpp, SCCS cpp.c 1.5, cpy.y 1.2, yylex.c 1.1), reconstructed after the cpp of cmd/cc
# in the Plexus tree. /lib/cpp is a combined program (code and data together, no -i).
#
#   make             cpp: crt0, the own code, the libc part: the shipped members (ar x from /lib/libc.a) and the old modules
#                    (cas -u of ../../lib/oldlibc), in the link order of /lib/cpp (tools/cpp.link, tools/cpp.spec)
#   make clean
#   (this file is written by tools/gen_makefile.py)
#
# The sources are written for the old compiler: make CC=ccv0p

CC = cc
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc
LDBIN = /bin/ld
CAS = cas
AR = ar
YACC = yacc
LIBC = /lib/libc.a
CFLAGS = -O

# shipped members in link order (the members of /lib/libc.a), extracted into shipped/
SHIPPED = ffltioc.o data.o __callex.o cuexit.o decimal.o brk.o close.o creat.o ioctl.o lseek.o read.o write.o cerror.o

# link order of the libc part of /lib/cpp
LIBCOBJS = $(OLD)/fopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_7.o shipped/ffltioc.o \
	$(OLD)/setbuf/0_1.o shipped/data.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_3.o $(OLD)/findiop/0_2.o \
	shipped/__callex.o shipped/cuexit.o $(OLD)/cnvt/0_2.o shipped/decimal.o $(OLD)/length/0_3.o \
	$(OLD)/malloc/0_3.o $(OLD)/isatty/0_2.o $(OLD)/stty/0_3.o $(OLD)/strcat/0_2.o $(OLD)/strcmp/0_1.o \
	$(OLD)/strcpy/0_2.o $(OLD)/strncpy/0_1.o shipped/brk.o shipped/close.o shipped/creat.o \
	shipped/ioctl.o shipped/lseek.o $(OLD)/open/0_1.o shipped/read.o shipped/write.o shipped/cerror.o
COBJS = cpp.o cpy.o
LINKOBJS = crt0.o $(COBJS) $(LIBCOBJS)

all: cpp

cpp: $(LINKOBJS)
	$(LDBIN) -e start $(LINKOBJS) -o $@

shipped/stamp:
	-mkdir shipped
	cd shipped ; $(AR) x $(LIBC) ffltioc.o data.o __callex.o cuexit.o decimal.o brk.o close.o creat.o ioctl.o lseek.o read.o write.o cerror.o ; touch stamp

cpp.o: cpp.c
	$(CC) $(CFLAGS) -c cpp.c

cpy.o: cpy.c yylex.c
	$(CC) $(CFLAGS) -c cpy.c

cpy.c: cpy.y
	$(YACC) cpy.y
	sed -e 's/^YYSTYPE yylval, yyval;/extern YYSTYPE yylval; YYSTYPE yyval;/' -e 's/^\(YYSTYPE yyv\[.*\)$$/\1\
YYSTYPE yylval;/' y.tab.c > cpy.c
	rm -f y.tab.c

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh cpp > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

shipped/ffltioc.o: shipped/stamp
shipped/data.o: shipped/stamp
shipped/__callex.o: shipped/stamp
shipped/cuexit.o: shipped/stamp
shipped/decimal.o: shipped/stamp
shipped/brk.o: shipped/stamp
shipped/close.o: shipped/stamp
shipped/creat.o: shipped/stamp
shipped/ioctl.o: shipped/stamp
shipped/lseek.o: shipped/stamp
shipped/read.o: shipped/stamp
shipped/write.o: shipped/stamp
shipped/cerror.o: shipped/stamp

$(OLD)/fopen/0_1.o: $(OLD)/fopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/fopen/0_1.s

$(OLD)/printf/0_5.o: $(OLD)/printf/0_5.s
	$(CAS) -u -o $@ $(OLD)/printf/0_5.s

$(OLD)/doprnt/0_7.o: $(OLD)/doprnt/0_7.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_7.s

$(OLD)/setbuf/0_1.o: $(OLD)/setbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/setbuf/0_1.s

$(OLD)/flsbuf/0_2.o: $(OLD)/flsbuf/0_2.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_2.s

$(OLD)/endopen/0_3.o: $(OLD)/endopen/0_3.s
	$(CAS) -u -o $@ $(OLD)/endopen/0_3.s

$(OLD)/findiop/0_2.o: $(OLD)/findiop/0_2.s
	$(CAS) -u -o $@ $(OLD)/findiop/0_2.s

$(OLD)/cnvt/0_2.o: $(OLD)/cnvt/0_2.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_2.s

$(OLD)/length/0_3.o: $(OLD)/length/0_3.s
	$(CAS) -u -o $@ $(OLD)/length/0_3.s

$(OLD)/malloc/0_3.o: $(OLD)/malloc/0_3.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_3.s

$(OLD)/isatty/0_2.o: $(OLD)/isatty/0_2.s
	$(CAS) -u -o $@ $(OLD)/isatty/0_2.s

$(OLD)/stty/0_3.o: $(OLD)/stty/0_3.s
	$(CAS) -u -o $@ $(OLD)/stty/0_3.s

$(OLD)/strcat/0_2.o: $(OLD)/strcat/0_2.s
	$(CAS) -u -o $@ $(OLD)/strcat/0_2.s

$(OLD)/strcmp/0_1.o: $(OLD)/strcmp/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcmp/0_1.s

$(OLD)/strcpy/0_2.o: $(OLD)/strcpy/0_2.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_2.s

$(OLD)/strncpy/0_1.o: $(OLD)/strncpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strncpy/0_1.s

$(OLD)/open/0_1.o: $(OLD)/open/0_1.s
	$(CAS) -u -o $@ $(OLD)/open/0_1.s

clean:
	rm -rf shipped
	rm -f $(LIBCOBJS)
	rm -f $(COBJS) crt0.s crt0.o cpy.c cpp
