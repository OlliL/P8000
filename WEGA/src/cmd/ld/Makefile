# ld: the segmented loader of WEGA (sld), reconstructed from /bin/sld.
#
#   make             compile the C files, assemble the start code and the old libc objects (printf, doprnt)
#   make link        sld.new: scrt0.o, the C files, the members of /lib/slibc.a and the two old objects, linked by the original
#                    sld (SLDBIN: segmented program, -i -z -e start)
#   make linkown     sld.own: the same with the cut own code (sld_own.s, 0x56..0x71ca of /bin/sld) instead of the C files
#   make clean
#   (this file is written by tools/gen_makefile.py)
#
# The libc part of sld (0x71ca..0x9730) consists of members of /lib/slibc.a in the order of /bin/sld; only printf and doprnt are
# another revision than the shipped members: their assembler sources are in ../../lib/oldslibc/<object>/<revision>.s (the
# revision number REV is invented). The code of the other objects equals the members (host/tools/oldslibc_cut.py checks that).

CRT = ../../lib/libc/crt
OLD = ../../lib/oldslibc
REV = 0_1
SLDBIN = /bin/sld
CAS = cas
AR = ar
SCC = scc
CFLAGS = -O
SLIBC = /lib/slibc.a

# the C files of sld (code ranges: tools/module_ranges.txt); each one is compiled alone
COBJS = copyname.o dump.o ext.o ld.o ldm.o printname.o swap.o symbols.o
# members of slibc.a (extracted into slibc/)
MEMBERS = putchar.o scanf.o doscan.o ffltioc.o ungetc.o filbuf.o flsbuf.o __callex.o cuexit.o atoi.o cnvt.o decimal.o length.o malloc.o isatty.o mktemp.o stty.o strchr.o strrchr.o swab.o rindex.o strcat.o strncmp.o strcpy.o swap.o access.o tolower.o chmod.o close.o creat.o fstat.o getpid.o ioctl.o link.o lseek.o mkseg.o open.o read.o ssgbrk.o signal.o umask.o unlink.o write.o cerror.o data.o ctype_.o
OLDOBJS = $(OLD)/printf/$(REV).o $(OLD)/doprnt/$(REV).o

# link order of /bin/sld
LIBCOBJS = slibc/putchar.o slibc/scanf.o slibc/doscan.o $(OLD)/printf/$(REV).o \
	$(OLD)/doprnt/$(REV).o slibc/ffltioc.o slibc/ungetc.o slibc/filbuf.o slibc/data.o slibc/flsbuf.o \
	slibc/__callex.o slibc/cuexit.o slibc/atoi.o slibc/cnvt.o slibc/decimal.o slibc/length.o \
	slibc/malloc.o slibc/ctype_.o slibc/isatty.o slibc/mktemp.o slibc/stty.o slibc/strchr.o \
	slibc/strrchr.o slibc/swab.o slibc/rindex.o slibc/strcat.o slibc/strncmp.o slibc/strcpy.o \
	slibc/swap.o slibc/access.o slibc/tolower.o slibc/chmod.o slibc/close.o slibc/creat.o \
	slibc/fstat.o slibc/getpid.o slibc/ioctl.o slibc/link.o slibc/lseek.o slibc/mkseg.o slibc/open.o \
	slibc/read.o slibc/ssgbrk.o slibc/signal.o slibc/umask.o slibc/unlink.o slibc/write.o \
	slibc/cerror.o
LINKOBJS = scrt0.o $(COBJS) $(LIBCOBJS)
OWNOBJS = scrt0.o sld_own.o $(LIBCOBJS)

all: $(COBJS) scrt0.o $(OLDOBJS)

link: sld.new

sld.new: $(LINKOBJS)
	$(SLDBIN) -i -z -e start $(LINKOBJS) -o $@

linkown: sld.own

sld.own: $(OWNOBJS)
	$(SLDBIN) -i -z -e start $(OWNOBJS) -o $@

slibc/stamp:
	mkdir slibc ; cd slibc ; $(AR) x $(SLIBC) $(MEMBERS) ; touch stamp

slibc/putchar.o: slibc/stamp
slibc/scanf.o: slibc/stamp
slibc/doscan.o: slibc/stamp
slibc/ffltioc.o: slibc/stamp
slibc/ungetc.o: slibc/stamp
slibc/filbuf.o: slibc/stamp
slibc/flsbuf.o: slibc/stamp
slibc/__callex.o: slibc/stamp
slibc/cuexit.o: slibc/stamp
slibc/atoi.o: slibc/stamp
slibc/cnvt.o: slibc/stamp
slibc/decimal.o: slibc/stamp
slibc/length.o: slibc/stamp
slibc/malloc.o: slibc/stamp
slibc/isatty.o: slibc/stamp
slibc/mktemp.o: slibc/stamp
slibc/stty.o: slibc/stamp
slibc/strchr.o: slibc/stamp
slibc/strrchr.o: slibc/stamp
slibc/swab.o: slibc/stamp
slibc/rindex.o: slibc/stamp
slibc/strcat.o: slibc/stamp
slibc/strncmp.o: slibc/stamp
slibc/strcpy.o: slibc/stamp
slibc/swap.o: slibc/stamp
slibc/access.o: slibc/stamp
slibc/tolower.o: slibc/stamp
slibc/chmod.o: slibc/stamp
slibc/close.o: slibc/stamp
slibc/creat.o: slibc/stamp
slibc/fstat.o: slibc/stamp
slibc/getpid.o: slibc/stamp
slibc/ioctl.o: slibc/stamp
slibc/link.o: slibc/stamp
slibc/lseek.o: slibc/stamp
slibc/mkseg.o: slibc/stamp
slibc/open.o: slibc/stamp
slibc/read.o: slibc/stamp
slibc/ssgbrk.o: slibc/stamp
slibc/signal.o: slibc/stamp
slibc/umask.o: slibc/stamp
slibc/unlink.o: slibc/stamp
slibc/write.o: slibc/stamp
slibc/cerror.o: slibc/stamp
slibc/data.o: slibc/stamp
slibc/ctype_.o: slibc/stamp

scrt0.s: $(CRT)/mkcrt0.sh $(CRT)/scrt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh sld > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

sld_own.o: sld_own.s
	$(CAS) -u -o $@ sld_own.s

copyname.o: copyname.c ld.h
	$(SCC) $(CFLAGS) -c copyname.c

dump.o: dump.c ld.h
	$(SCC) $(CFLAGS) -c dump.c

ext.o: ext.c ld.h
	$(SCC) $(CFLAGS) -c ext.c

ld.o: ld.c ld.h
	$(SCC) $(CFLAGS) -c ld.c

ldm.o: ldm.c ld.h
	$(SCC) $(CFLAGS) -c ldm.c

printname.o: printname.c ld.h
	$(SCC) $(CFLAGS) -c printname.c

swap.o: swap.c ld.h
	$(SCC) $(CFLAGS) -c swap.c

symbols.o: symbols.c ld.h
	$(SCC) $(CFLAGS) -c symbols.c

$(OLD)/printf/$(REV).o: $(OLD)/printf/$(REV).s
	$(CAS) -u -o $@ $(OLD)/printf/$(REV).s

$(OLD)/doprnt/$(REV).o: $(OLD)/doprnt/$(REV).s
	$(CAS) -u -o $@ $(OLD)/doprnt/$(REV).s

clean:
	rm -f $(COBJS) $(OLDOBJS) scrt0.s scrt0.o sld_own.o sld.new sld.own
	rm -rf slibc
