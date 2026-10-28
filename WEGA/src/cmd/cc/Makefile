# cc and scc, the drivers. cc.c is the source of both: scc is cc.c compiled with -DSCC (the object is
# named after the source file). Both are linked from their own objects, the old libc modules they were linked
# with originally (../../lib/oldlibc) and the members of the shipped libc.a that are identical to the original
# ones: no libc is linked. cc and scc have the same libc part; the order of the objects is the one of cc.link
# and scc.link (made by host/tools/unlink.py from cc.spec and scc.spec); see ../../lib/oldlibc/README.
#
#   make              build ./cc and ./scc
#   make install PREFIX=dir
#   make clean
#
# The result is byte-identical with the original /bin/cc and /bin/scc (see README).
CAS = cas
CC  = cc
LD  = ld
AR  = ar
CFLAGS = -O
PREFIX = /
LIBC = /lib/libc.a
OLD = ../../lib/oldlibc

OBJS = freopen.o fopen.o $(OLD)/printf/0_4.o \
	$(OLD)/doprnt/0_4.o ffltioc.o flsbuf.o \
	endopen.o findiop.o data.o \
	__callex.o cuexit.o abort.o \
	$(OLD)/cnvt/0_1.o decimal.o $(OLD)/length/0_1.o \
	$(OLD)/malloc/0_4.o ctype_.o isatty.o \
	stty.o $(OLD)/strcat/0_1.o strcmp.o \
	$(OLD)/strcpy/0_1.o $(OLD)/strlen/0_1.o access.o \
	brk.o chmod.o close.o \
	creat.o execl.o execv.o \
	fork.o getpid.o ioctl.o \
	lseek.o open.o signal.o \
	stat.o umask.o unlink.o \
	wait.o write.o cerror.o

all: cc scc

cc: cc.o $(OBJS)
	$(LD) -s -e start -o $@ /lib/crt0.o cc.o $(OBJS)

scc: scc.o $(OBJS)
	$(LD) -s -e start -o $@ /lib/crt0.o scc.o $(OBJS)

cc.o: cc.c
	$(CC) $(CFLAGS) -c cc.c

scc.o: cc.c
	cp cc.c scc.c
	$(CC) $(CFLAGS) -DSCC -c scc.c
	rm -f scc.c

# members of the shipped libc
freopen.o fopen.o flsbuf.o ffltioc.o endopen.o findiop.o data.o __callex.o cuexit.o abort.o decimal.o ctype_.o isatty.o stty.o strcmp.o access.o brk.o chmod.o close.o creat.o execl.o execv.o fork.o getpid.o ioctl.o lseek.o open.o signal.o stat.o umask.o unlink.o wait.o write.o cerror.o:
	$(AR) x $(LIBC) freopen.o fopen.o flsbuf.o ffltioc.o endopen.o findiop.o data.o __callex.o cuexit.o abort.o decimal.o ctype_.o isatty.o stty.o strcmp.o access.o brk.o chmod.o close.o creat.o execl.o execv.o fork.o getpid.o ioctl.o lseek.o open.o signal.o stat.o umask.o unlink.o wait.o write.o cerror.o

# modules of the old libc
$(OLD)/printf/0_4.o: $(OLD)/printf/0_4.s
	$(CAS) -u -o $@ $(OLD)/printf/0_4.s

$(OLD)/doprnt/0_4.o: $(OLD)/doprnt/0_4.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_4.s

$(OLD)/cnvt/0_1.o: $(OLD)/cnvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_1.s

$(OLD)/length/0_1.o: $(OLD)/length/0_1.s
	$(CAS) -u -o $@ $(OLD)/length/0_1.s

$(OLD)/malloc/0_4.o: $(OLD)/malloc/0_4.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_4.s

$(OLD)/strcat/0_1.o: $(OLD)/strcat/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcat/0_1.s

$(OLD)/strcpy/0_1.o: $(OLD)/strcpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_1.s

$(OLD)/strlen/0_1.o: $(OLD)/strlen/0_1.s
	$(CAS) -u -o $@ $(OLD)/strlen/0_1.s

install: cc scc
	cp cc $(PREFIX)/bin/cc
	cp scc $(PREFIX)/bin/scc

clean:
	rm -f cc scc cc.o scc.o scc.c freopen.o fopen.o flsbuf.o endopen.o findiop.o data.o __callex.o cuexit.o abort.o decimal.o ctype_.o isatty.o stty.o strcmp.o access.o brk.o chmod.o close.o creat.o execl.o execv.o fork.o getpid.o ioctl.o lseek.o open.o signal.o stat.o umask.o unlink.o wait.o write.o cerror.o
	rm -f $(OLD)/printf/0_4.o $(OLD)/doprnt/0_4.o $(OLD)/cnvt/0_1.o $(OLD)/length/0_1.o $(OLD)/malloc/0_4.o $(OLD)/strcat/0_1.o $(OLD)/strcpy/0_1.o $(OLD)/strlen/0_1.o

.PHONY: all install clean
