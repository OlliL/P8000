# cparse: pass 1 of cc (PCC front end) of WEGA, reconstructed from /lib/cparse (E711, nonsegmented, stripped).
#
#   make block       step 3 of host/HOWTO_reconstruct.md: the own part of the original as one raw module (made from
#                    the original by tools/rawblock.py, needs WEGA=root of the original tree), linked with the libc part
#                    (LIBCOBJS); the image of cparse_block must be equal to /lib/cparse (make checkblock)
#   make             cparse from our sources: crt0.o (made by mkcrt0.sh cparse), the modules in the order of the original, the libc part
#   make old         the versions of the old compiler in OLDDIRS (v1/Makefile ...)
#   make clean
#
# The parser cgram1.y is made into cgram1.c by the original yacc (YACC). bss.s and libcbss.s (only .comm lines) give
# the bss the order of the original. The libc part: shipped members of /lib/libc.a (ar x into shipped/) and versions of
# src/lib/oldlibc, in the link order of /lib/cparse (tools/cparse_libc_split.spec, HANDOFF_cparse.md step 2).

CRT = ../../lib/libc/crt
CAS = cas
LDBIN = /bin/ld
WEGA = ../../../../tmp/wega31_root

CC = cc
YACC = yacc
CFLAGS = -O
OBJS = bss.o cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o
HDRS = mfile1 manifest macdefs stdio.h
OLD = ../../lib/oldlibc
AR = ar
LIBC = /lib/libc.a
# the libc part of /lib/cparse in its link order (tools/cparse_libc_split.spec): shipped members of /lib/libc.a and
# the versions of src/lib/oldlibc
LIBCOBJS = libcbss.o $(OLD)/fltioc/0_1.o $(OLD)/fseek/0_2.o $(OLD)/ftell/0_2.o $(OLD)/rdwr/0_3.o $(OLD)/fopen/0_1.o $(OLD)/fdopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o $(OLD)/ungetc/0_1.o shipped/atof.o $(OLD)/ecvt/0_1.o shipped/pow10.o $(OLD)/filbuf/0_3.o shipped/data.o $(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_4.o $(OLD)/findiop/0_2.o shipped/__callex.o shipped/cuexit.o $(OLD)/perror/0_1.o $(OLD)/cnvt/0_2.o shipped/decimal.o $(OLD)/length/0_3.o $(OLD)/malloc/0_3.o $(OLD)/errlst/0_1.o $(OLD)/isatty/0_2.o $(OLD)/stty/0_3.o $(OLD)/strcat/0_2.o $(OLD)/strcmp/0_1.o $(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o shipped/ctype_.o shipped/brk.o shipped/chmod.o shipped/close.o shipped/creat.o shipped/ioctl.o shipped/lseek.o shipped/open.o shipped/read.o shipped/signal.o shipped/umask.o shipped/write.o shipped/cerror.o

# the versions of the old compiler (one directory each, own Makefile; not part of all)
OLDDIRS = v1

all: cparse

old: all
	for d in $(OLDDIRS); do cd $$d ; $(MAKE) ; cd .. ; done

cparse: crt0.o $(OBJS) $(LIBCOBJS)
	$(LDBIN) -i -e start crt0.o $(OBJS) $(LIBCOBJS) -o $@

$(OBJS): $(HDRS)
optim1.o: optim
comm1.o: common

.c.o:
	$(CC) $(CFLAGS) -c $<

bss.o: bss.s
	$(CAS) -u -o $@ bss.s

libcbss.o: libcbss.s
	$(CAS) -u -o $@ libcbss.s

cgram1.c: cgram1.y
	$(YACC) cgram1.y
	mv y.tab.c cgram1.c

block: cparse_block

cparse_block: crt0.o ownraw.o $(LIBCOBJS)
	$(LDBIN) -i -e start crt0.o ownraw.o $(LIBCOBJS) -o $@

ownraw.s: tools/rawblock.py
	WEGA=$(WEGA) python3 tools/rawblock.py $@

ownraw.o: ownraw.s
	$(CAS) -u -o $@ ownraw.s

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh cparse > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

$(OLD)/fltioc/0_1.o: $(OLD)/fltioc/0_1.s
	$(CAS) -u -o $@ $(OLD)/fltioc/0_1.s

$(OLD)/fseek/0_2.o: $(OLD)/fseek/0_2.s
	$(CAS) -u -o $@ $(OLD)/fseek/0_2.s

$(OLD)/ftell/0_2.o: $(OLD)/ftell/0_2.s
	$(CAS) -u -o $@ $(OLD)/ftell/0_2.s

$(OLD)/rdwr/0_3.o: $(OLD)/rdwr/0_3.s
	$(CAS) -u -o $@ $(OLD)/rdwr/0_3.s

$(OLD)/fopen/0_1.o: $(OLD)/fopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/fopen/0_1.s

$(OLD)/fdopen/0_1.o: $(OLD)/fdopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/fdopen/0_1.s

$(OLD)/printf/0_5.o: $(OLD)/printf/0_5.s
	$(CAS) -u -o $@ $(OLD)/printf/0_5.s

$(OLD)/doprnt/0_5.o: $(OLD)/doprnt/0_5.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_5.s

$(OLD)/ungetc/0_1.o: $(OLD)/ungetc/0_1.s
	$(CAS) -u -o $@ $(OLD)/ungetc/0_1.s

$(OLD)/ecvt/0_1.o: $(OLD)/ecvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/ecvt/0_1.s

$(OLD)/filbuf/0_3.o: $(OLD)/filbuf/0_3.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_3.s

$(OLD)/setbuf/0_1.o: $(OLD)/setbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/setbuf/0_1.s

$(OLD)/flsbuf/0_2.o: $(OLD)/flsbuf/0_2.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_2.s

$(OLD)/endopen/0_4.o: $(OLD)/endopen/0_4.s
	$(CAS) -u -o $@ $(OLD)/endopen/0_4.s

$(OLD)/findiop/0_2.o: $(OLD)/findiop/0_2.s
	$(CAS) -u -o $@ $(OLD)/findiop/0_2.s

$(OLD)/perror/0_1.o: $(OLD)/perror/0_1.s
	$(CAS) -u -o $@ $(OLD)/perror/0_1.s

$(OLD)/cnvt/0_2.o: $(OLD)/cnvt/0_2.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_2.s

$(OLD)/length/0_3.o: $(OLD)/length/0_3.s
	$(CAS) -u -o $@ $(OLD)/length/0_3.s

$(OLD)/malloc/0_3.o: $(OLD)/malloc/0_3.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_3.s

$(OLD)/errlst/0_1.o: $(OLD)/errlst/0_1.s
	$(CAS) -u -o $@ $(OLD)/errlst/0_1.s

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

$(OLD)/strlen/0_2.o: $(OLD)/strlen/0_2.s
	$(CAS) -u -o $@ $(OLD)/strlen/0_2.s

shipped/stamp:
	-mkdir shipped
	cd shipped ; $(AR) x $(LIBC) atof.o pow10.o data.o __callex.o cuexit.o decimal.o ctype_.o brk.o chmod.o close.o creat.o ioctl.o lseek.o open.o read.o signal.o umask.o write.o cerror.o ; touch stamp

shipped/atof.o: shipped/stamp
shipped/pow10.o: shipped/stamp
shipped/data.o: shipped/stamp
shipped/__callex.o: shipped/stamp
shipped/cuexit.o: shipped/stamp
shipped/decimal.o: shipped/stamp
shipped/ctype_.o: shipped/stamp
shipped/brk.o: shipped/stamp
shipped/chmod.o: shipped/stamp
shipped/close.o: shipped/stamp
shipped/creat.o: shipped/stamp
shipped/ioctl.o: shipped/stamp
shipped/lseek.o: shipped/stamp
shipped/open.o: shipped/stamp
shipped/read.o: shipped/stamp
shipped/signal.o: shipped/stamp
shipped/umask.o: shipped/stamp
shipped/write.o: shipped/stamp
shipped/cerror.o: shipped/stamp

checkblock: cparse_block
	python3 tools/cmpimage.py cparse_block $(WEGA)/lib/cparse

clean:
	rm -f ownraw.s ownraw.o crt0.s cgram1.c cparse_block cparse *.o
	rm -rf shipped
