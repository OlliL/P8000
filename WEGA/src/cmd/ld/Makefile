# ld: the segmented loader of WEGA (sld, gld), reconstructed from /bin/sld. This Makefile builds the libc of sld.
# The libc of sld is not /lib/slibc.a: its modules are the cut versions of the original sld (../../lib/oldslibc,
# cut and verified with host/tools/unlink_seg.py). Only ffltioc, getpid and umask are shipped members of
# /lib/slibc.a, identical to the original code (taken with ar x, like popt does).
#
#   make              libc objects of sld, the own code of sld (sld_own.o) and the start-up code (scrt0.o)
#   make link         sld.new, the segmented program linked from these objects by the original sld (SLDBIN)
#   make clean

# start-up code of sld: written by mkcrt0.sh from the template scrt0.s.in (row sld in crt0.list); the first 62 bytes
# of sld (0x18..0x55), link it before sld_own.o
CRT = ../../lib/libc/crt
# linker for the test link: the original sld (segmented program: -i -z -e start)
SLDBIN = /bin/sld
# link order of sld as in the libc README (ffltioc at 0x8680, getpid at 0x95aa, umask at 0x96f2)
LINKOBJS = scrt0.o sld_own.o ffltioc.o $(OLD)/filbuf/0_1.o $(OLD)/flsbuf/0_1.o $(OLD)/cnvt/0_1.o \
	$(OLD)/decimal/0_1.o $(OLD)/malloc/0_1.o $(OLD)/isatty/0_1.o $(OLD)/mktemp/0_1.o $(OLD)/stty/0_1.o \
	$(OLD)/swab/0_1.o $(OLD)/strcmp/0_1.o $(OLD)/strcpy/0_1.o $(OLD)/swap/0_1.o getpid.o \
	$(OLD)/signal/0_1.o umask.o $(OLD)/stubs/0_1.o

CAS = cas
AR = ar
SLIBC = /lib/slibc.a
OLD = ../../lib/oldslibc

# cut modules of the libc of sld (ranges of sld: see ../../lib/oldslibc/README)
LIBCOBJS = $(OLD)/filbuf/0_1.o $(OLD)/flsbuf/0_1.o $(OLD)/cnvt/0_1.o $(OLD)/decimal/0_1.o \
	$(OLD)/malloc/0_1.o $(OLD)/isatty/0_1.o $(OLD)/mktemp/0_1.o $(OLD)/stty/0_1.o \
	$(OLD)/swab/0_1.o $(OLD)/strcmp/0_1.o $(OLD)/strcpy/0_1.o $(OLD)/swap/0_1.o \
	$(OLD)/signal/0_1.o $(OLD)/stubs/0_1.o
# shipped members of slibc.a that are identical to the original code of sld
SHIPOBJS = ffltioc.o getpid.o umask.o

all: libc own start

# the link: not byte identical yet (the data of sld is not cut, the libc data spans are at other offsets)
link: sld.new

sld.new: $(LINKOBJS)
	$(SLDBIN) -i -z -e start $(LINKOBJS) -o $@

libc: $(LIBCOBJS) $(SHIPOBJS)

# own code of sld (0x56..0x8680 of /bin/sld, one block; cut with host/tools/unlink_seg.py, verified)
own: sld_own.o

# start-up code (0x18..0x55 of /bin/sld)
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

$(OLD)/filbuf/0_1.o: $(OLD)/filbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_1.s

$(OLD)/flsbuf/0_1.o: $(OLD)/flsbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_1.s

$(OLD)/cnvt/0_1.o: $(OLD)/cnvt/0_1.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_1.s

$(OLD)/decimal/0_1.o: $(OLD)/decimal/0_1.s
	$(CAS) -u -o $@ $(OLD)/decimal/0_1.s

$(OLD)/malloc/0_1.o: $(OLD)/malloc/0_1.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_1.s

$(OLD)/isatty/0_1.o: $(OLD)/isatty/0_1.s
	$(CAS) -u -o $@ $(OLD)/isatty/0_1.s

$(OLD)/mktemp/0_1.o: $(OLD)/mktemp/0_1.s
	$(CAS) -u -o $@ $(OLD)/mktemp/0_1.s

$(OLD)/stty/0_1.o: $(OLD)/stty/0_1.s
	$(CAS) -u -o $@ $(OLD)/stty/0_1.s

$(OLD)/swab/0_1.o: $(OLD)/swab/0_1.s
	$(CAS) -u -o $@ $(OLD)/swab/0_1.s

$(OLD)/strcmp/0_1.o: $(OLD)/strcmp/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcmp/0_1.s

$(OLD)/strcpy/0_1.o: $(OLD)/strcpy/0_1.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_1.s

$(OLD)/swap/0_1.o: $(OLD)/swap/0_1.s
	$(CAS) -u -o $@ $(OLD)/swap/0_1.s

$(OLD)/signal/0_1.o: $(OLD)/signal/0_1.s
	$(CAS) -u -o $@ $(OLD)/signal/0_1.s

$(OLD)/stubs/0_1.o: $(OLD)/stubs/0_1.s
	$(CAS) -u -o $@ $(OLD)/stubs/0_1.s

clean:
	rm -f $(LIBCOBJS) $(SHIPOBJS) sld_own.o scrt0.s scrt0.o sld.new
