# popt: the peephole optimizer of cc (second pass), reconstructed from /lib/popt (K&R sources, see
# ../../../host/popt/NOTES.md and src/TODO.md). The libc of popt is not linked from /lib/libc.a: its members are
# the versions of the original (VERSIONS in oldlibc: the members that the older compiler built, cut from the
# original) and the members of the shipped libc.a that are identical to the original ones (ar x), like codgen does.
#
#   make              popt
#   make clean

CC = cc
CAS = cas
LD = ld
AR = ar
CFLAGS = -O
LIBC = /lib/libc.a
CRT0 = /lib/crt0.o
OLD = ../../lib/oldlibc

# members of oldlibc (the versions of the original popt, see VERSIONS)
OLDOBJS = $(OLD)/freopen/0_1.o $(OLD)/printf/0_5.o $(OLD)/doprnt/0_5.o $(OLD)/filbuf/0_3.o \
	$(OLD)/setbuf/0_1.o $(OLD)/flsbuf/0_2.o $(OLD)/endopen/0_4.o $(OLD)/cnvt/0_2.o \
	$(OLD)/length/0_2.o $(OLD)/malloc/0_3.o $(OLD)/isatty/0_2.o $(OLD)/stty/0_2.o \
	$(OLD)/strcat/0_2.o $(OLD)/strncmp/0_1.o $(OLD)/strcpy/0_2.o $(OLD)/strlen/0_2.o \
	$(OLD)/atoi/0_1.o $(OLD)/strrchr/0_1.o
# members of the shipped libc.a that are identical to the original ones
SHIPOBJS = __callex.o data.o brk.o cuexit.o ffltioc.o write.o close.o read.o \
	lseek.o decimal.o cerror.o open.o creat.o ioctl.o umask.o chmod.o ctype_.o strchr.o strcmp.o
OBJS = z20.o z21.o z22.o z23.o

all: popt

popt: $(OBJS) $(OLDOBJS) ship
	$(LD) -X -e start -i crt0.o $(OBJS) $(OLDOBJS) $(SHIPOBJS) -o popt

$(OBJS): z2.h

# the start-up file of popt is the shipped /lib/crt0.o (same code: start, 42 bytes)
ship:
	cp $(CRT0) crt0.o
	$(AR) x $(LIBC) $(SHIPOBJS)

$(OLD)/freopen/0_1.o: $(OLD)/freopen/0_1.s
	$(CAS) -u -o $@ $(OLD)/freopen/0_1.s

$(OLD)/printf/0_5.o: $(OLD)/printf/0_5.s
	$(CAS) -u -o $@ $(OLD)/printf/0_5.s

$(OLD)/doprnt/0_5.o: $(OLD)/doprnt/0_5.s
	$(CAS) -u -o $@ $(OLD)/doprnt/0_5.s

$(OLD)/filbuf/0_3.o: $(OLD)/filbuf/0_3.s
	$(CAS) -u -o $@ $(OLD)/filbuf/0_3.s

$(OLD)/setbuf/0_1.o: $(OLD)/setbuf/0_1.s
	$(CAS) -u -o $@ $(OLD)/setbuf/0_1.s

$(OLD)/flsbuf/0_2.o: $(OLD)/flsbuf/0_2.s
	$(CAS) -u -o $@ $(OLD)/flsbuf/0_2.s

$(OLD)/endopen/0_4.o: $(OLD)/endopen/0_4.s
	$(CAS) -u -o $@ $(OLD)/endopen/0_4.s

$(OLD)/cnvt/0_2.o: $(OLD)/cnvt/0_2.s
	$(CAS) -u -o $@ $(OLD)/cnvt/0_2.s

$(OLD)/length/0_2.o: $(OLD)/length/0_2.s
	$(CAS) -u -o $@ $(OLD)/length/0_2.s

$(OLD)/malloc/0_3.o: $(OLD)/malloc/0_3.s
	$(CAS) -u -o $@ $(OLD)/malloc/0_3.s

$(OLD)/isatty/0_2.o: $(OLD)/isatty/0_2.s
	$(CAS) -u -o $@ $(OLD)/isatty/0_2.s

$(OLD)/stty/0_2.o: $(OLD)/stty/0_2.s
	$(CAS) -u -o $@ $(OLD)/stty/0_2.s

$(OLD)/strcat/0_2.o: $(OLD)/strcat/0_2.s
	$(CAS) -u -o $@ $(OLD)/strcat/0_2.s

$(OLD)/strncmp/0_1.o: $(OLD)/strncmp/0_1.s
	$(CAS) -u -o $@ $(OLD)/strncmp/0_1.s

$(OLD)/strcpy/0_2.o: $(OLD)/strcpy/0_2.s
	$(CAS) -u -o $@ $(OLD)/strcpy/0_2.s

$(OLD)/strlen/0_2.o: $(OLD)/strlen/0_2.s
	$(CAS) -u -o $@ $(OLD)/strlen/0_2.s

$(OLD)/atoi/0_1.o: $(OLD)/atoi/0_1.s
	$(CAS) -u -o $@ $(OLD)/atoi/0_1.s

$(OLD)/strrchr/0_1.o: $(OLD)/strrchr/0_1.s
	$(CAS) -u -o $@ $(OLD)/strrchr/0_1.s

clean:
	-rm -f $(OBJS) $(OLDOBJS) $(SHIPOBJS) crt0.o popt
