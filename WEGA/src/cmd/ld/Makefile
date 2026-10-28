# ld: the segmented loader of WEGA (sld, gld), reconstructed from /bin/sld. This Makefile builds the libc part of sld.
# The libc part of sld (0x8682..0x9730) is a sequence of members of slibc.a, in the order the original linker took them. Every
# object has a directory in ../../lib/oldslibc/<object>/<revision>.s, cut from /bin/sld and verified with
# host/tools/oldslibc_cut.py (the revision number REV is invented: other programs carry other revisions of the same objects).
# ffltioc, getpid and umask (6 to 8 bytes) are shipped members of /lib/slibc.a, taken with ar x. The code of all objects equals
# the members of /lib/slibc.a (oldslibc_cut.py checks that); the data is not cut exactly yet (see ../../lib/oldslibc/README).
#
#   make              libc objects of sld, the own code of sld (sld_own.o) and the start-up code (scrt0.o)
#   make link         sld.new, linked from these objects by the original sld (SLDBIN)
#   make linkslibc    sld.new.slibc: same link, but the objects are the members of /lib/slibc.a
#   make clean

CRT = ../../lib/libc/crt
OLD = ../../lib/oldslibc
REV = 0_1
# linker for the test link: the original sld (segmented program: -i -z -e start)
SLDBIN = /bin/sld
CAS = cas
AR = ar
SLIBC = /lib/slibc.a

# shipped members of slibc.a that are identical to the original code of sld
SHIPOBJS = ffltioc.o getpid.o umask.o
# link order of sld (= address order in /bin/sld): start code, own code, then the libc objects
LINKOBJS = scrt0.o sld_own.o \
	ffltioc.o $(OLD)/ungetc/$(REV).o $(OLD)/filbuf/$(REV).o $(OLD)/flsbuf/$(REV).o $(OLD)/__callex/$(REV).o  \
	$(OLD)/cuexit/$(REV).o $(OLD)/atoi/$(REV).o $(OLD)/cnvt/$(REV).o $(OLD)/decimal/$(REV).o  \
	$(OLD)/length/$(REV).o $(OLD)/malloc/$(REV).o $(OLD)/isatty/$(REV).o $(OLD)/mktemp/$(REV).o  \
	$(OLD)/stty/$(REV).o $(OLD)/strchr/$(REV).o $(OLD)/strrchr/$(REV).o $(OLD)/swab/$(REV).o  \
	$(OLD)/rindex/$(REV).o $(OLD)/strcat/$(REV).o $(OLD)/strncmp/$(REV).o $(OLD)/strcpy/$(REV).o  \
	$(OLD)/swap/$(REV).o $(OLD)/access/$(REV).o $(OLD)/tolower/$(REV).o $(OLD)/chmod/$(REV).o  \
	$(OLD)/close/$(REV).o $(OLD)/creat/$(REV).o $(OLD)/fstat/$(REV).o getpid.o $(OLD)/ioctl/$(REV).o  \
	$(OLD)/link/$(REV).o $(OLD)/lseek/$(REV).o $(OLD)/mkseg/$(REV).o $(OLD)/open/$(REV).o $(OLD)/read/$(REV).o  \
	$(OLD)/ssgbrk/$(REV).o $(OLD)/signal/$(REV).o umask.o $(OLD)/unlink/$(REV).o $(OLD)/write/$(REV).o  \
	$(OLD)/cerror/$(REV).o
# the same with all objects taken from slibc.a
SLIBCOBJS = scrt0.o sld_own.o \
	slibc/ffltioc.o slibc/ungetc.o slibc/filbuf.o slibc/flsbuf.o slibc/__callex.o slibc/cuexit.o \
	slibc/atoi.o slibc/cnvt.o slibc/decimal.o slibc/length.o slibc/malloc.o slibc/isatty.o \
	slibc/mktemp.o slibc/stty.o slibc/strchr.o slibc/strrchr.o slibc/swab.o slibc/rindex.o \
	slibc/strcat.o slibc/strncmp.o slibc/strcpy.o slibc/swap.o slibc/access.o slibc/tolower.o \
	slibc/chmod.o slibc/close.o slibc/creat.o slibc/fstat.o slibc/getpid.o slibc/ioctl.o slibc/link.o \
	slibc/lseek.o slibc/mkseg.o slibc/open.o slibc/read.o slibc/ssgbrk.o slibc/signal.o slibc/umask.o \
	slibc/unlink.o slibc/write.o slibc/cerror.o slibc/data.o

LIBCOBJS = $(OLD)/ungetc/$(REV).o $(OLD)/filbuf/$(REV).o $(OLD)/flsbuf/$(REV).o $(OLD)/__callex/$(REV).o  \
	$(OLD)/cuexit/$(REV).o $(OLD)/atoi/$(REV).o $(OLD)/cnvt/$(REV).o $(OLD)/decimal/$(REV).o  \
	$(OLD)/length/$(REV).o $(OLD)/malloc/$(REV).o $(OLD)/isatty/$(REV).o $(OLD)/mktemp/$(REV).o  \
	$(OLD)/stty/$(REV).o $(OLD)/strchr/$(REV).o $(OLD)/strrchr/$(REV).o $(OLD)/swab/$(REV).o  \
	$(OLD)/rindex/$(REV).o $(OLD)/strcat/$(REV).o $(OLD)/strncmp/$(REV).o $(OLD)/strcpy/$(REV).o  \
	$(OLD)/swap/$(REV).o $(OLD)/access/$(REV).o $(OLD)/tolower/$(REV).o $(OLD)/chmod/$(REV).o  \
	$(OLD)/close/$(REV).o $(OLD)/creat/$(REV).o $(OLD)/fstat/$(REV).o $(OLD)/ioctl/$(REV).o $(OLD)/link/$(REV).o  \
	$(OLD)/lseek/$(REV).o $(OLD)/mkseg/$(REV).o $(OLD)/open/$(REV).o $(OLD)/read/$(REV).o $(OLD)/ssgbrk/$(REV).o  \
	$(OLD)/signal/$(REV).o $(OLD)/unlink/$(REV).o $(OLD)/write/$(REV).o $(OLD)/cerror/$(REV).o

all: libc own start

libc: $(LIBCOBJS) $(SHIPOBJS)

# the link: not byte identical yet (the data of sld is not cut, the libc data spans are at other offsets)
link: sld.new

sld.new: $(LINKOBJS)
	$(SLDBIN) -i -z -e start $(LINKOBJS) -o $@

linkslibc: sld.new.slibc

slibc/stamp:
	mkdir slibc ; cd slibc ; $(AR) x $(SLIBC) ffltioc.o ungetc.o filbuf.o flsbuf.o __callex.o cuexit.o atoi.o cnvt.o decimal.o length.o malloc.o isatty.o mktemp.o stty.o strchr.o strrchr.o swab.o rindex.o strcat.o strncmp.o strcpy.o swap.o access.o tolower.o chmod.o close.o creat.o fstat.o getpid.o ioctl.o link.o lseek.o mkseg.o open.o read.o ssgbrk.o signal.o umask.o unlink.o write.o cerror.o data.o ; touch stamp

sld.new.slibc: slibc/stamp $(SLIBCOBJS)
	$(SLDBIN) -i -z -e start $(SLIBCOBJS) -o $@

# own code of sld (0x56..0x8682 of /bin/sld, one block; cut with host/tools/oldslibc_cut.py, verified)
own: sld_own.o

# start-up code (0x18..0x55 of /bin/sld): written by mkcrt0.sh from the template scrt0.s.in (row sld in crt0.list)
start: scrt0.o

scrt0.s: $(CRT)/mkcrt0.sh $(CRT)/scrt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh sld > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

sld_own.o: sld_own.s
	$(CAS) -u -o $@ sld_own.s

ffltioc.o:
	$(AR) x $(SLIBC) ffltioc.o

getpid.o:
	$(AR) x $(SLIBC) getpid.o

umask.o:
	$(AR) x $(SLIBC) umask.o

$(OLD)/ungetc/$(REV).o: $(OLD)/ungetc/$(REV).s
	$(CAS) -u -o $@ $(OLD)/ungetc/$(REV).s

$(OLD)/filbuf/$(REV).o: $(OLD)/filbuf/$(REV).s
	$(CAS) -u -o $@ $(OLD)/filbuf/$(REV).s

$(OLD)/flsbuf/$(REV).o: $(OLD)/flsbuf/$(REV).s
	$(CAS) -u -o $@ $(OLD)/flsbuf/$(REV).s

$(OLD)/__callex/$(REV).o: $(OLD)/__callex/$(REV).s
	$(CAS) -u -o $@ $(OLD)/__callex/$(REV).s

$(OLD)/cuexit/$(REV).o: $(OLD)/cuexit/$(REV).s
	$(CAS) -u -o $@ $(OLD)/cuexit/$(REV).s

$(OLD)/atoi/$(REV).o: $(OLD)/atoi/$(REV).s
	$(CAS) -u -o $@ $(OLD)/atoi/$(REV).s

$(OLD)/cnvt/$(REV).o: $(OLD)/cnvt/$(REV).s
	$(CAS) -u -o $@ $(OLD)/cnvt/$(REV).s

$(OLD)/decimal/$(REV).o: $(OLD)/decimal/$(REV).s
	$(CAS) -u -o $@ $(OLD)/decimal/$(REV).s

$(OLD)/length/$(REV).o: $(OLD)/length/$(REV).s
	$(CAS) -u -o $@ $(OLD)/length/$(REV).s

$(OLD)/malloc/$(REV).o: $(OLD)/malloc/$(REV).s
	$(CAS) -u -o $@ $(OLD)/malloc/$(REV).s

$(OLD)/isatty/$(REV).o: $(OLD)/isatty/$(REV).s
	$(CAS) -u -o $@ $(OLD)/isatty/$(REV).s

$(OLD)/mktemp/$(REV).o: $(OLD)/mktemp/$(REV).s
	$(CAS) -u -o $@ $(OLD)/mktemp/$(REV).s

$(OLD)/stty/$(REV).o: $(OLD)/stty/$(REV).s
	$(CAS) -u -o $@ $(OLD)/stty/$(REV).s

$(OLD)/strchr/$(REV).o: $(OLD)/strchr/$(REV).s
	$(CAS) -u -o $@ $(OLD)/strchr/$(REV).s

$(OLD)/strrchr/$(REV).o: $(OLD)/strrchr/$(REV).s
	$(CAS) -u -o $@ $(OLD)/strrchr/$(REV).s

$(OLD)/swab/$(REV).o: $(OLD)/swab/$(REV).s
	$(CAS) -u -o $@ $(OLD)/swab/$(REV).s

$(OLD)/rindex/$(REV).o: $(OLD)/rindex/$(REV).s
	$(CAS) -u -o $@ $(OLD)/rindex/$(REV).s

$(OLD)/strcat/$(REV).o: $(OLD)/strcat/$(REV).s
	$(CAS) -u -o $@ $(OLD)/strcat/$(REV).s

$(OLD)/strncmp/$(REV).o: $(OLD)/strncmp/$(REV).s
	$(CAS) -u -o $@ $(OLD)/strncmp/$(REV).s

$(OLD)/strcpy/$(REV).o: $(OLD)/strcpy/$(REV).s
	$(CAS) -u -o $@ $(OLD)/strcpy/$(REV).s

$(OLD)/swap/$(REV).o: $(OLD)/swap/$(REV).s
	$(CAS) -u -o $@ $(OLD)/swap/$(REV).s

$(OLD)/access/$(REV).o: $(OLD)/access/$(REV).s
	$(CAS) -u -o $@ $(OLD)/access/$(REV).s

$(OLD)/tolower/$(REV).o: $(OLD)/tolower/$(REV).s
	$(CAS) -u -o $@ $(OLD)/tolower/$(REV).s

$(OLD)/chmod/$(REV).o: $(OLD)/chmod/$(REV).s
	$(CAS) -u -o $@ $(OLD)/chmod/$(REV).s

$(OLD)/close/$(REV).o: $(OLD)/close/$(REV).s
	$(CAS) -u -o $@ $(OLD)/close/$(REV).s

$(OLD)/creat/$(REV).o: $(OLD)/creat/$(REV).s
	$(CAS) -u -o $@ $(OLD)/creat/$(REV).s

$(OLD)/fstat/$(REV).o: $(OLD)/fstat/$(REV).s
	$(CAS) -u -o $@ $(OLD)/fstat/$(REV).s

$(OLD)/ioctl/$(REV).o: $(OLD)/ioctl/$(REV).s
	$(CAS) -u -o $@ $(OLD)/ioctl/$(REV).s

$(OLD)/link/$(REV).o: $(OLD)/link/$(REV).s
	$(CAS) -u -o $@ $(OLD)/link/$(REV).s

$(OLD)/lseek/$(REV).o: $(OLD)/lseek/$(REV).s
	$(CAS) -u -o $@ $(OLD)/lseek/$(REV).s

$(OLD)/mkseg/$(REV).o: $(OLD)/mkseg/$(REV).s
	$(CAS) -u -o $@ $(OLD)/mkseg/$(REV).s

$(OLD)/open/$(REV).o: $(OLD)/open/$(REV).s
	$(CAS) -u -o $@ $(OLD)/open/$(REV).s

$(OLD)/read/$(REV).o: $(OLD)/read/$(REV).s
	$(CAS) -u -o $@ $(OLD)/read/$(REV).s

$(OLD)/ssgbrk/$(REV).o: $(OLD)/ssgbrk/$(REV).s
	$(CAS) -u -o $@ $(OLD)/ssgbrk/$(REV).s

$(OLD)/signal/$(REV).o: $(OLD)/signal/$(REV).s
	$(CAS) -u -o $@ $(OLD)/signal/$(REV).s

$(OLD)/unlink/$(REV).o: $(OLD)/unlink/$(REV).s
	$(CAS) -u -o $@ $(OLD)/unlink/$(REV).s

$(OLD)/write/$(REV).o: $(OLD)/write/$(REV).s
	$(CAS) -u -o $@ $(OLD)/write/$(REV).s

$(OLD)/cerror/$(REV).o: $(OLD)/cerror/$(REV).s
	$(CAS) -u -o $@ $(OLD)/cerror/$(REV).s

clean:
	rm -f $(LIBCOBJS) $(SHIPOBJS) sld_own.o scrt0.s scrt0.o sld.new sld.new.slibc
	rm -rf slibc
