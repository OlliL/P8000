# ld: the segmented loader of WEGA (sld, gld), reconstructed from /bin/sld. This Makefile builds the libc of sld.
# The libc of sld is not /lib/slibc.a: its modules are the cut versions of the original sld (../../lib/oldslibc,
# cut and verified with host/tools/unlink_seg.py). Only ffltioc, getpid and umask are shipped members of
# /lib/slibc.a, identical to the original code (taken with ar x, like popt does).
#
#   make              libc objects of sld and the own code of sld (sld_own.o)
#   make clean

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

all: libc own

libc: $(LIBCOBJS) $(SHIPOBJS)

# own code of sld (0x18..0x8680 of /bin/sld, one block; cut with host/tools/unlink_seg.py, verified)
own: sld_own.o

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
	rm -f $(LIBCOBJS) $(SHIPOBJS) sld_own.o
