# size, linked from its own objects, the old libc modules it was linked with originally (../../lib/oldlibc)
# and the members of the shipped libc.a that are identical to the original ones: no libc is linked.
# The order of the objects is the one of size.link (made by host/tools/unlink.py from size.spec); see
# ../../lib/oldlibc/README.
#
#   make              build ./size
#   make install PREFIX=dir
#   make clean
#
# The result is not byte-identical with the original /bin/size yet: size.o of the original was made by
# an older cc that does not pad the string literals to even addresses (D12 in host/KNOWLEDGE.md), the
# shipped cc does, so the data is 2 bytes longer. With that object the file is identical (checked).
CAS = cas
CC  = cc
LD  = ld
AR  = ar
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc

OBJS = $(OLD)/rdwr/0_1.o fopen.o $(OLD)/printf/0_1.o \
	$(OLD)/doprnt/0_1.o ffltioc.o $(OLD)/filbuf/0_1.o \
	$(OLD)/flsbuf/0_1.o $(OLD)/endopen/0_1.o $(OLD)/findiop/0_1.o \
	data.o __callex.o cuexit.o \
	$(OLD)/cnvt/0_1.o decimal.o $(OLD)/length/0_1.o \
	$(OLD)/malloc/0_1.o isatty.o stty.o \
	swab.o $(OLD)/goodmagic/0_1.o $(OLD)/swap/0_1.o \
	brk.o chmod.o close.o \
	creat.o ioctl.o lseek.o \
	open.o read.o umask.o \
	write.o cerror.o

all: size

size: size.o $(CRT)/crt0-size.o $(OBJS)
	$(LD) -s -e start -o $@ $(CRT)/crt0-size.o size.o $(OBJS)

size.o: size.c
	$(CC) $(CFLAGS) -c size.c

$(CRT)/crt0-size.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh size > $@

$(CRT)/crt0-size.o: $(CRT)/crt0-size.s
	$(CAS) -u -o $@ $(CRT)/crt0-size.s

# members of the shipped libc
fopen.o ffltioc.o data.o __callex.o cuexit.o decimal.o isatty.o stty.o swab.o brk.o chmod.o close.o creat.o ioctl.o lseek.o open.o read.o umask.o write.o cerror.o:
	$(AR) x $(LIBC) fopen.o ffltioc.o data.o __callex.o cuexit.o decimal.o isatty.o stty.o swab.o brk.o chmod.o close.o creat.o ioctl.o lseek.o open.o read.o umask.o write.o cerror.o

# modules of the old libc
$(OLD)/rdwr/0_1.o: $(OLD)/rdwr/0_1.s
	$(CAS) -u -o $@ $(OLD)/rdwr/0_1.s

$(OLD)/printf/0_1.o: $(OLD)/printf/0_1.s
	$(CAS) -u -o $@ $(OLD)/printf/0_1.s

$(OLD)/doprnt/0_1.o: $(OLD)/doprnt/0_1.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_1.s

$(OLD)/filbuf/0_1.o: $(OLD)/filbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_1.s

$(OLD)/flsbuf/0_1.o: $(OLD)/flsbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_1.s

$(OLD)/endopen/0_1.o: $(OLD)/endopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/endopen/0_1.s

$(OLD)/findiop/0_1.o: $(OLD)/findiop/0_1.s
	$(CAS) -u -o $@ $(OLD)/findiop/0_1.s

$(OLD)/cnvt/0_1.o: $(OLD)/cnvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_1.s

$(OLD)/length/0_1.o: $(OLD)/length/0_1.s
	$(CAS) -u -o $@ $(OLD)/length/0_1.s

$(OLD)/malloc/0_1.o: $(OLD)/malloc/0_1.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_1.s

$(OLD)/goodmagic/0_1.o: $(OLD)/goodmagic/0_1.s
	$(CAS) -u -o $@ $(OLD)/goodmagic/0_1.s

$(OLD)/swap/0_1.o: $(OLD)/swap/0_1.s
	$(CAS) -u -o $@ $(OLD)/swap/0_1.s

install: size
	cp size $(PREFIX)/bin/size

clean:
	rm -f size size.o fopen.o data.o __callex.o cuexit.o decimal.o isatty.o stty.o swab.o brk.o chmod.o close.o creat.o ioctl.o lseek.o open.o read.o umask.o write.o cerror.o
	rm -f $(CRT)/crt0-size.s $(CRT)/crt0-size.o
	rm -f $(OLD)/rdwr/0_1.o $(OLD)/printf/0_1.o $(OLD)/doprnt/0_1.o $(OLD)/filbuf/0_1.o $(OLD)/flsbuf/0_1.o $(OLD)/endopen/0_1.o $(OLD)/findiop/0_1.o $(OLD)/cnvt/0_1.o $(OLD)/length/0_1.o $(OLD)/malloc/0_1.o $(OLD)/goodmagic/0_1.o $(OLD)/swap/0_1.o

.PHONY: all install clean
