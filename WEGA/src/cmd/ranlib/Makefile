# ranlib, linked from its own objects, the old libc modules it was linked with originally (../../lib/oldlibc)
# and the members of the shipped libc.a that are identical to the original ones: no libc is linked.
# The order of the objects is the one of ranlib.link (made by host/tools/unlink.py from ranlib.spec); see
# ../../lib/oldlibc/README. readsym.c is the C form of the old readsym; the build uses the assembler module.
#
#   make              build ./ranlib
#   make install PREFIX=dir
#   make clean
#
# The result is not byte-identical with the original /usr/bin/ranlib yet: ranlib.o of the original was made
# by an older cc (own code differs, D1, D2, D4, and the string literals are not padded to even addresses, D12).
# The libc part is the one of the original (checked with tools/cmpins.py).
CAS = cas
CC  = cc
LD  = ld
AR  = ar
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc

OBJS = $(OLD)/fseek/0_2.o $(OLD)/ftell/0_1.o $(OLD)/readsym/0_1.o \
	$(OLD)/rdwr/0_3.o $(OLD)/system/0_1.o $(OLD)/fopen/0_1.o \
	$(OLD)/printf/0_3.o $(OLD)/doprnt/0_3.o ffltioc.o \
	$(OLD)/filbuf/0_3.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_3.o \
	$(OLD)/findiop/0_2.o data.o __callex.o \
	cuexit.o $(OLD)/cnvt/0_2.o decimal.o \
	$(OLD)/length/0_2.o $(OLD)/malloc/0_3.o $(OLD)/isatty/0_1.o \
	$(OLD)/stty/0_1.o $(OLD)/swab/0_1.o $(OLD)/strncmp/0_1.o \
	$(OLD)/strncpy/0_1.o $(OLD)/goodmagic/0_2.o $(OLD)/swap/0_2.o \
	brk.o close.o creat.o \
	execl.o exit.o fork.o \
	ioctl.o lseek.o $(OLD)/open/0_1.o \
	read.o signal.o time.o \
	unlink.o wait.o write.o \
	cerror.o

all: ranlib

ranlib: ranlib.o $(CRT)/crt0-ranlib.o $(OBJS)
	$(LD) -s -e start -o $@ $(CRT)/crt0-ranlib.o ranlib.o $(OBJS)

ranlib.o: ranlib.c
	$(CC) $(CFLAGS) -c ranlib.c

$(CRT)/crt0-ranlib.s: $(CRT)/crt0.s.in $(CRT)/crt0.list $(CRT)/mkcrt0.sh
	sh $(CRT)/mkcrt0.sh ranlib > $@

$(CRT)/crt0-ranlib.o: $(CRT)/crt0-ranlib.s
	$(CAS) -u -o $@ $(CRT)/crt0-ranlib.s

# members of the shipped libc
ffltioc.o data.o __callex.o cuexit.o decimal.o brk.o close.o creat.o execl.o exit.o fork.o ioctl.o lseek.o read.o signal.o time.o unlink.o wait.o write.o cerror.o:
	$(AR) x $(LIBC) ffltioc.o data.o __callex.o cuexit.o decimal.o brk.o close.o creat.o execl.o exit.o fork.o ioctl.o lseek.o read.o signal.o time.o unlink.o wait.o write.o cerror.o

# modules of the old libc
$(OLD)/fseek/0_2.o: $(OLD)/fseek/0_2.s
	$(CAS) -u -o $@ $(OLD)/fseek/0_2.s

$(OLD)/ftell/0_1.o: $(OLD)/ftell/0_1.s
	$(CAS) -u -o $@ $(OLD)/ftell/0_1.s

$(OLD)/readsym/0_1.o: $(OLD)/readsym/0_1.s
	$(CAS) -u -o $@ $(OLD)/readsym/0_1.s

$(OLD)/rdwr/0_3.o: $(OLD)/rdwr/0_3.s
	$(CAS) -u -o $@ $(OLD)/rdwr/0_3.s

$(OLD)/system/0_1.o: $(OLD)/system/0_1.s
	$(CAS) -u -o $@ $(OLD)/system/0_1.s

$(OLD)/fopen/0_1.o: $(OLD)/fopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/fopen/0_1.s

$(OLD)/printf/0_3.o: $(OLD)/printf/0_3.s
	$(CAS) -u -o $@ $(OLD)/printf/0_3.s

$(OLD)/doprnt/0_3.o: $(OLD)/doprnt/0_3.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_3.s

$(OLD)/filbuf/0_3.o: $(OLD)/filbuf/0_3.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_3.s

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

$(OLD)/swab/0_1.o: $(OLD)/swab/0_1.s
	$(CAS) -u -o $@ $(OLD)/swab/0_1.s

$(OLD)/strncmp/0_1.o: $(OLD)/strncmp/0_1.s
	$(CAS) -u -o $@ $(OLD)/strncmp/0_1.s

$(OLD)/strncpy/0_1.o: $(OLD)/strncpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strncpy/0_1.s

$(OLD)/goodmagic/0_2.o: $(OLD)/goodmagic/0_2.s
	$(CAS) -u -o $@ $(OLD)/goodmagic/0_2.s

$(OLD)/swap/0_2.o: $(OLD)/swap/0_2.s
	$(CAS) -u -o $@ $(OLD)/swap/0_2.s

$(OLD)/open/0_1.o: $(OLD)/open/0_1.s
	$(CAS) -u -o $@ $(OLD)/open/0_1.s

install: ranlib
	cp ranlib $(PREFIX)/usr/bin/ranlib

clean:
	rm -f ranlib ranlib.o data.o __callex.o cuexit.o decimal.o brk.o close.o creat.o execl.o exit.o fork.o ioctl.o lseek.o read.o signal.o time.o unlink.o wait.o write.o cerror.o
	rm -f $(CRT)/crt0-ranlib.s $(CRT)/crt0-ranlib.o
	rm -f $(OLD)/fseek/0_2.o $(OLD)/ftell/0_1.o $(OLD)/readsym/0_1.o $(OLD)/rdwr/0_3.o $(OLD)/system/0_1.o $(OLD)/fopen/0_1.o $(OLD)/printf/0_3.o $(OLD)/doprnt/0_3.o $(OLD)/filbuf/0_3.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_3.o $(OLD)/findiop/0_2.o $(OLD)/cnvt/0_2.o $(OLD)/length/0_2.o $(OLD)/malloc/0_3.o $(OLD)/isatty/0_1.o $(OLD)/stty/0_1.o $(OLD)/swab/0_1.o $(OLD)/strncmp/0_1.o $(OLD)/strncpy/0_1.o $(OLD)/goodmagic/0_2.o $(OLD)/swap/0_2.o $(OLD)/open/0_1.o

.PHONY: all install clean
