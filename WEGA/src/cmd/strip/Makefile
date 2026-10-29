# strip, linked from its own objects, the old libc modules it was linked with originally (../../lib/oldlibc)
# and the members of the shipped libc.a that are identical to the original ones: no libc is linked.
# The order of the objects is the one of strip.link (made by host/tools/unlink.py from strip.spec); see
# ../../lib/oldlibc/README.
#
#   make              build ./strip
#   make install PREFIX=dir
#   make clean
#
# The result is not byte-identical with the original /usr/bin/strip yet: strip.o of the original was made
# by an older cc (own code differs in 2 blocks, D2, and the string literals are not padded to even
# addresses, D12). The libc part is the one of the original (checked with tools/cmpins.py).
CAS = cas
CC  = cc
LD  = ld
AR  = ar
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc

OBJS = $(OLD)/fseek/0_1.o $(OLD)/rdwr/0_2.o fopen.o \
	$(OLD)/printf/0_2.o $(OLD)/doprnt/0_2.o ffltioc.o \
	$(OLD)/filbuf/0_2.o $(OLD)/flsbuf/0_1.o $(OLD)/endopen/0_2.o \
	$(OLD)/findiop/0_1.o data.o __callex.o \
	cuexit.o $(OLD)/cnvt/0_1.o decimal.o \
	$(OLD)/length/0_1.o $(OLD)/malloc/0_2.o isatty.o \
	$(OLD)/mktemp/0_1.o stty.o swab.o \
	strcmp.o $(OLD)/strcpy/0_1.o $(OLD)/goodmagic/0_1.o \
	$(OLD)/swap/0_1.o access.o brk.o \
	chmod.o close.o creat.o \
	getpid.o ioctl.o lseek.o \
	open.o read.o signal.o \
	umask.o unlink.o write.o \
	cerror.o

all: strip

strip: strip.o $(CRT)/crt0-strip.o $(OBJS)
	$(LD) -s -e start -o $@ $(CRT)/crt0-strip.o strip.o $(OBJS)

strip.o: strip.c
	$(CC) $(CFLAGS) -c strip.c

$(CRT)/crt0-strip.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh strip > $@

$(CRT)/crt0-strip.o: $(CRT)/crt0-strip.s
	$(CAS) -u -o $@ $(CRT)/crt0-strip.s

# members of the shipped libc
fopen.o ffltioc.o data.o __callex.o cuexit.o decimal.o isatty.o stty.o swab.o strcmp.o access.o brk.o chmod.o close.o creat.o getpid.o ioctl.o lseek.o open.o read.o signal.o umask.o unlink.o write.o cerror.o:
	$(AR) x $(LIBC) fopen.o ffltioc.o data.o __callex.o cuexit.o decimal.o isatty.o stty.o swab.o strcmp.o access.o brk.o chmod.o close.o creat.o getpid.o ioctl.o lseek.o open.o read.o signal.o umask.o unlink.o write.o cerror.o

# modules of the old libc
$(OLD)/fseek/0_1.o: $(OLD)/fseek/0_1.s
	$(CAS) -u -o $@ $(OLD)/fseek/0_1.s

$(OLD)/rdwr/0_2.o: $(OLD)/rdwr/0_2.s
	$(CAS) -u -o $@ $(OLD)/rdwr/0_2.s

$(OLD)/printf/0_2.o: $(OLD)/printf/0_2.s
	$(CAS) -u -o $@ $(OLD)/printf/0_2.s

$(OLD)/doprnt/0_2.o: $(OLD)/doprnt/0_2.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_2.s

$(OLD)/filbuf/0_2.o: $(OLD)/filbuf/0_2.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_2.s

$(OLD)/flsbuf/0_1.o: $(OLD)/flsbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_1.s

$(OLD)/endopen/0_2.o: $(OLD)/endopen/0_2.s
	$(CAS) -u -o $@ $(OLD)/endopen/0_2.s

$(OLD)/findiop/0_1.o: $(OLD)/findiop/0_1.s
	$(CAS) -u -o $@ $(OLD)/findiop/0_1.s

$(OLD)/cnvt/0_1.o: $(OLD)/cnvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_1.s

$(OLD)/length/0_1.o: $(OLD)/length/0_1.s
	$(CAS) -u -o $@ $(OLD)/length/0_1.s

$(OLD)/malloc/0_2.o: $(OLD)/malloc/0_2.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_2.s

$(OLD)/mktemp/0_1.o: $(OLD)/mktemp/0_1.s
	$(CAS) -u -o $@ $(OLD)/mktemp/0_1.s

$(OLD)/strcpy/0_1.o: $(OLD)/strcpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_1.s

$(OLD)/goodmagic/0_1.o: $(OLD)/goodmagic/0_1.s
	$(CAS) -u -o $@ $(OLD)/goodmagic/0_1.s

$(OLD)/swap/0_1.o: $(OLD)/swap/0_1.s
	$(CAS) -u -o $@ $(OLD)/swap/0_1.s

install: strip
	cp strip $(PREFIX)/usr/bin/strip

clean:
	rm -f strip strip.o fopen.o data.o __callex.o cuexit.o decimal.o isatty.o stty.o swab.o strcmp.o access.o brk.o chmod.o close.o creat.o getpid.o ioctl.o lseek.o open.o read.o signal.o umask.o unlink.o write.o cerror.o
	rm -f $(CRT)/crt0-strip.s $(CRT)/crt0-strip.o
	rm -f $(OLD)/fseek/0_1.o $(OLD)/rdwr/0_2.o $(OLD)/printf/0_2.o $(OLD)/doprnt/0_2.o $(OLD)/filbuf/0_2.o $(OLD)/flsbuf/0_1.o $(OLD)/endopen/0_2.o $(OLD)/findiop/0_1.o $(OLD)/cnvt/0_1.o $(OLD)/length/0_1.o $(OLD)/malloc/0_2.o $(OLD)/mktemp/0_1.o $(OLD)/strcpy/0_1.o $(OLD)/goodmagic/0_1.o $(OLD)/swap/0_1.o

.PHONY: all install clean
