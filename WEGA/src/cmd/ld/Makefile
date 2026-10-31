# ld: the nonsegmented loader of WEGA (/bin/ld, E711, SCCS ld.c 1.18), reconstructed from /bin/ld.
#
#   make             the libc part of ld: the shipped members (ar x from /lib/libc.a) and the old modules (cas -u of
#                    ../../lib/oldlibc), in the link order of /bin/ld (tools/ld.link, tools/ld.spec)
#   make check       tools/libc_verify.py: every module against the original bytes of /bin/ld (needs the file tree of the
#                    original system, WEGA=dir)
#   make clean
#   (this file is written by tools/gen_makefile.py; the own code of ld is not in it yet: step 3)
#
# make link builds ld; the sources are written for the old compiler v1 (make link CC=ccv1, ../cc/v1: 54 of the
# 59 own functions strictly exact, data equal, see tools/HANDOFF_ld.md "Old compiler v1").
# The own code (copyname.c, printname.c, symbols.c, ld.c, crt0 from mkcrt0.sh) comes later. Relocated words of the
# old modules point to the own code and the data of ld; they are checked when the link exists.

CRT = ../../lib/libc/crt
OLD = ../../lib/oldlibc
LDBIN = /bin/ld
CAS = cas
AR = ar
LIBC = /lib/libc.a
CFLAGS = -O
WEGA = ../../../tmp/wega31_root

# shipped members in link order (the members of /lib/libc.a), extracted into shipped/
SHIPPED = scanf.o ffltioc.o ungetc.o data.o __callex.o cuexit.o decimal.o isatty.o ctype_.o stty.o swab.o tolower.o strncmp.o access.o brk.o chmod.o close.o creat.o fstat.o getpid.o ioctl.o link.o lseek.o open.o read.o signal.o umask.o unlink.o write.o cerror.o

# link order of the libc part of /bin/ld
LIBCOBJS = shipped/scanf.o $(OLD)/doscan/0_1.o $(OLD)/printf/0_6.o $(OLD)/doprnt/0_6.o \
	shipped/ffltioc.o shipped/ungetc.o $(OLD)/filbuf/0_2.o shipped/data.o $(OLD)/flsbuf/0_1.o \
	shipped/__callex.o shipped/cuexit.o $(OLD)/cnvt/0_1.o shipped/decimal.o $(OLD)/length/0_1.o \
	$(OLD)/malloc/0_2.o shipped/isatty.o $(OLD)/mktemp/0_1.o shipped/ctype_.o shipped/stty.o \
	shipped/swab.o $(OLD)/rindex/0_1.o shipped/tolower.o $(OLD)/strcat/0_1.o shipped/strncmp.o \
	$(OLD)/strcpy/0_1.o $(OLD)/strncpy/0_2.o $(OLD)/swap/0_1.o shipped/access.o shipped/brk.o \
	shipped/chmod.o shipped/close.o shipped/creat.o shipped/fstat.o shipped/getpid.o shipped/ioctl.o \
	shipped/link.o shipped/lseek.o shipped/open.o shipped/read.o shipped/signal.o shipped/umask.o \
	shipped/unlink.o shipped/write.o shipped/cerror.o
COBJS = ld.o copyname.o printname.o symbols.o
LINKOBJS = crt0.o $(COBJS) $(LIBCOBJS)

all: crt0.o $(OLDOBJS)

link: ld

ld: $(COBJS) $(LINKOBJS)
	$(LDBIN) -i -e start $(LINKOBJS) -o $@

shipped/stamp:
	-mkdir shipped
	cd shipped ; $(AR) x $(LIBC) scanf.o ffltioc.o ungetc.o data.o __callex.o cuexit.o decimal.o isatty.o ctype_.o stty.o swab.o tolower.o strncmp.o access.o brk.o chmod.o close.o creat.o fstat.o getpid.o ioctl.o link.o lseek.o open.o read.o signal.o umask.o unlink.o write.o cerror.o ; touch stamp


copyname.o: copyname.c ld.h
	$(CC) $(CFLAGS) -c copyname.c

ld.o: ld.c ld.h
	$(CC) $(CFLAGS) -c ld.c

printname.o: printname.c ld.h
	$(CC) $(CFLAGS) -c printname.c

symbols.o: symbols.c ld.h
	$(CC) $(CFLAGS) -c symbols.c

crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh ld > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ crt0.s
	
shipped/scanf.o: shipped/stamp
shipped/ffltioc.o: shipped/stamp
shipped/ungetc.o: shipped/stamp
shipped/data.o: shipped/stamp
shipped/__callex.o: shipped/stamp
shipped/cuexit.o: shipped/stamp
shipped/decimal.o: shipped/stamp
shipped/isatty.o: shipped/stamp
shipped/ctype_.o: shipped/stamp
shipped/stty.o: shipped/stamp
shipped/swab.o: shipped/stamp
shipped/tolower.o: shipped/stamp
shipped/strncmp.o: shipped/stamp
shipped/access.o: shipped/stamp
shipped/brk.o: shipped/stamp
shipped/chmod.o: shipped/stamp
shipped/close.o: shipped/stamp
shipped/creat.o: shipped/stamp
shipped/fstat.o: shipped/stamp
shipped/getpid.o: shipped/stamp
shipped/ioctl.o: shipped/stamp
shipped/link.o: shipped/stamp
shipped/lseek.o: shipped/stamp
shipped/open.o: shipped/stamp
shipped/read.o: shipped/stamp
shipped/signal.o: shipped/stamp
shipped/umask.o: shipped/stamp
shipped/unlink.o: shipped/stamp
shipped/write.o: shipped/stamp
shipped/cerror.o: shipped/stamp

$(OLD)/doscan/0_1.o: $(OLD)/doscan/0_1.s
	$(CAS) -u -o $@ $(OLD)/doscan/0_1.s

$(OLD)/printf/0_6.o: $(OLD)/printf/0_6.s
	$(CAS) -u -o $@ $(OLD)/printf/0_6.s

$(OLD)/doprnt/0_6.o: $(OLD)/doprnt/0_6.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_6.s

$(OLD)/filbuf/0_2.o: $(OLD)/filbuf/0_2.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_2.s

$(OLD)/flsbuf/0_1.o: $(OLD)/flsbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_1.s

$(OLD)/cnvt/0_1.o: $(OLD)/cnvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_1.s

$(OLD)/length/0_1.o: $(OLD)/length/0_1.s
	$(CAS) -u -o $@ $(OLD)/length/0_1.s

$(OLD)/malloc/0_2.o: $(OLD)/malloc/0_2.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_2.s

$(OLD)/mktemp/0_1.o: $(OLD)/mktemp/0_1.s
	$(CAS) -u -o $@ $(OLD)/mktemp/0_1.s

$(OLD)/rindex/0_1.o: $(OLD)/rindex/0_1.s
	$(CAS) -u -o $@ $(OLD)/rindex/0_1.s

$(OLD)/strcat/0_1.o: $(OLD)/strcat/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcat/0_1.s

$(OLD)/strcpy/0_1.o: $(OLD)/strcpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_1.s

$(OLD)/strncpy/0_2.o: $(OLD)/strncpy/0_2.s
	$(CAS) -u -o $@ $(OLD)/strncpy/0_2.s

$(OLD)/swap/0_1.o: $(OLD)/swap/0_1.s
	$(CAS) -u -o $@ $(OLD)/swap/0_1.s

check:
	cd tools ; WEGA=$(WEGA) python3 libc_verify.py

clean:
	rm -rf shipped
	rm -f $(LIBCOBJS)
	rm -f $(COBJS)
