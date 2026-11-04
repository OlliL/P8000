# dc: linked from its own objects and the libc part of the original (dc.link, ../../lib/oldlibc): no libc is linked.
# With the old compiler (make CC=ccv0) and strip the result is byte-identical with the original.
# Written by host/tools/linkmk.py.
#
#   make              build ./dc
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

OWN = dc1.o dc2.o
LIBCOBJS = $(OLD)/fopen/0_1.o $(OLD)/printf/0_3.o $(OLD)/doprnt/0_3.o shipped/ffltioc.o \
	$(OLD)/ungetc/0_1.o $(OLD)/filbuf/0_3.o $(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o \
	$(OLD)/endopen/0_3.o $(OLD)/findiop/0_2.o shipped/data.o shipped/__callex.o shipped/cuexit.o \
	shipped/abort.o $(OLD)/cnvt/0_2.o shipped/decimal.o $(OLD)/length/0_2.o $(OLD)/malloc/0_3.o \
	$(OLD)/isatty/0_1.o $(OLD)/stty/0_1.o shipped/brk.o shipped/close.o shipped/creat.o \
	shipped/execl.o shipped/fork.o shipped/ioctl.o shipped/lseek.o $(OLD)/open/0_1.o shipped/read.o \
	shipped/signal.o shipped/wait.o shipped/write.o shipped/cerror.o

all: dc

dc: crt0.o $(OWN) $(LIBCOBJS)
	$(LD) -s -e start -o $@ crt0.o $(OWN) $(LIBCOBJS)

crt0.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh dc > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s

dc1.o: dc1.c dc.h
	$(CC) $(CFLAGS) -c dc1.c

dc2.o: dc2.c dc.h
	$(CC) $(CFLAGS) -c dc2.c

# members of the shipped libc
SHIPPED = ffltioc.o data.o __callex.o cuexit.o abort.o decimal.o \
	brk.o close.o creat.o execl.o fork.o ioctl.o lseek.o read.o signal.o wait.o write.o cerror.o

shipped/stamp:
	-mkdir shipped
	cd shipped ; $(AR) x $(LIBC) $(SHIPPED) ; touch stamp

shipped/ffltioc.o: shipped/stamp
shipped/data.o: shipped/stamp
shipped/__callex.o: shipped/stamp
shipped/cuexit.o: shipped/stamp
shipped/abort.o: shipped/stamp
shipped/decimal.o: shipped/stamp
shipped/brk.o: shipped/stamp
shipped/close.o: shipped/stamp
shipped/creat.o: shipped/stamp
shipped/execl.o: shipped/stamp
shipped/fork.o: shipped/stamp
shipped/ioctl.o: shipped/stamp
shipped/lseek.o: shipped/stamp
shipped/read.o: shipped/stamp
shipped/signal.o: shipped/stamp
shipped/wait.o: shipped/stamp
shipped/write.o: shipped/stamp
shipped/cerror.o: shipped/stamp

# modules of the old libc
$(OLD)/fopen/0_1.o: $(OLD)/fopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/fopen/0_1.s

$(OLD)/printf/0_3.o: $(OLD)/printf/0_3.s
	$(CAS) -u -o $@ $(OLD)/printf/0_3.s

$(OLD)/doprnt/0_3.o: $(OLD)/doprnt/0_3.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_3.s

$(OLD)/ungetc/0_1.o: $(OLD)/ungetc/0_1.s
	$(CAS) -u -o $@ $(OLD)/ungetc/0_1.s

$(OLD)/filbuf/0_3.o: $(OLD)/filbuf/0_3.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_3.s

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

$(OLD)/length/0_2.o: $(OLD)/length/0_2.s
	$(CAS) -u -o $@ $(OLD)/length/0_2.s

$(OLD)/malloc/0_3.o: $(OLD)/malloc/0_3.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_3.s

$(OLD)/isatty/0_1.o: $(OLD)/isatty/0_1.s
	$(CAS) -u -o $@ $(OLD)/isatty/0_1.s

$(OLD)/stty/0_1.o: $(OLD)/stty/0_1.s
	$(CAS) -u -o $@ $(OLD)/stty/0_1.s

$(OLD)/open/0_1.o: $(OLD)/open/0_1.s
	$(CAS) -u -o $@ $(OLD)/open/0_1.s

install: dc
	cp dc $(PREFIX)/bin/dc

clean:
	rm -rf *.o crt0.s shipped dc

.PHONY: all install clean
