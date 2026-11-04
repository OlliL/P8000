# mv: linked from its own objects and the libc part of the original (mv.link, ../../lib/oldlibc): no libc is linked.
# With the old compiler (make CC=ccv1) and strip the result is byte-identical with the original.
# Written by host/tools/linkmk.py.
#
#   make              build ./mv
#   make install PREFIX=dir
#   make clean

CC = cc
CAS = cas
LD = ld
AR = ar
CFLAGS = -O -I../cparse
PREFIX = /
LIBC = /lib/libc.a
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc

OWN = mv.o
LIBCOBJS = $(OLD)/printf/0_6.o $(OLD)/doprnt/0_6.o shipped/ffltioc.o $(OLD)/filbuf/0_2.o \
	$(OLD)/flsbuf/0_1.o shipped/data.o shipped/__callex.o shipped/cuexit.o $(OLD)/perror/0_2.o \
	$(OLD)/cnvt/0_1.o shipped/decimal.o $(OLD)/length/0_1.o $(OLD)/malloc/0_2.o $(OLD)/errlst/0_2.o \
	shipped/isatty.o shipped/stty.o $(OLD)/strrchr/0_2.o shipped/strcmp.o $(OLD)/strcpy/0_1.o \
	$(OLD)/strlen/0_1.o shipped/access.o shipped/brk.o shipped/chmod.o shipped/close.o shipped/creat.o \
	shipped/getuid.o shipped/ioctl.o shipped/link.o shipped/open.o shipped/read.o shipped/setuid.o \
	shipped/stat.o shipped/unlink.o shipped/write.o shipped/cerror.o

all: mv

mv: crt0.o $(OWN) $(LIBCOBJS)
	$(LD) -s -e start -o $@ crt0.o $(OWN) $(LIBCOBJS)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh mv > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

mv.o: mv.c
	$(CC) $(CFLAGS) -c mv.c

# members of the shipped libc
SHIPPED = ffltioc.o data.o __callex.o cuexit.o decimal.o isatty.o \
	stty.o strcmp.o access.o brk.o chmod.o close.o creat.o getuid.o ioctl.o link.o open.o read.o \
	setuid.o stat.o unlink.o write.o cerror.o

shipped/stamp:
	-mkdir shipped
	cd shipped ; $(AR) x $(LIBC) $(SHIPPED) ; touch stamp

shipped/ffltioc.o: shipped/stamp
shipped/data.o: shipped/stamp
shipped/__callex.o: shipped/stamp
shipped/cuexit.o: shipped/stamp
shipped/decimal.o: shipped/stamp
shipped/isatty.o: shipped/stamp
shipped/stty.o: shipped/stamp
shipped/strcmp.o: shipped/stamp
shipped/access.o: shipped/stamp
shipped/brk.o: shipped/stamp
shipped/chmod.o: shipped/stamp
shipped/close.o: shipped/stamp
shipped/creat.o: shipped/stamp
shipped/getuid.o: shipped/stamp
shipped/ioctl.o: shipped/stamp
shipped/link.o: shipped/stamp
shipped/open.o: shipped/stamp
shipped/read.o: shipped/stamp
shipped/setuid.o: shipped/stamp
shipped/stat.o: shipped/stamp
shipped/unlink.o: shipped/stamp
shipped/write.o: shipped/stamp
shipped/cerror.o: shipped/stamp

# modules of the old libc
$(OLD)/printf/0_6.o: $(OLD)/printf/0_6.s
	$(CAS) -u -o $@ $(OLD)/printf/0_6.s

$(OLD)/doprnt/0_6.o: $(OLD)/doprnt/0_6.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_6.s

$(OLD)/filbuf/0_2.o: $(OLD)/filbuf/0_2.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_2.s

$(OLD)/flsbuf/0_1.o: $(OLD)/flsbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_1.s

$(OLD)/perror/0_2.o: $(OLD)/perror/0_2.s
	$(CAS) -u -o $@ $(OLD)/perror/0_2.s

$(OLD)/cnvt/0_1.o: $(OLD)/cnvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_1.s

$(OLD)/length/0_1.o: $(OLD)/length/0_1.s
	$(CAS) -u -o $@ $(OLD)/length/0_1.s

$(OLD)/malloc/0_2.o: $(OLD)/malloc/0_2.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_2.s

$(OLD)/errlst/0_2.o: $(OLD)/errlst/0_2.s
	$(CAS) -u -o $@ $(OLD)/errlst/0_2.s

$(OLD)/strrchr/0_2.o: $(OLD)/strrchr/0_2.s
	$(CAS) -u -o $@ $(OLD)/strrchr/0_2.s

$(OLD)/strcpy/0_1.o: $(OLD)/strcpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_1.s

$(OLD)/strlen/0_1.o: $(OLD)/strlen/0_1.s
	$(CAS) -u -o $@ $(OLD)/strlen/0_1.s

install: mv
	cp mv $(PREFIX)/bin/mv

clean:
	rm -rf *.o crt0.s shipped mv

.PHONY: all install clean
