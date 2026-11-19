# libu8000: /usr/lib/libu8000.a and /usr/lib/slibu8000.a (disasm(3): the U8000 disassembler that adb and cas also carry,
# nonsegmented and segmented). No source is known: disasm.c is written from the member disasm.o. Both libraries are made
# from the same source by v1 (ccv1, sccv1 from OLDROOT) without -O; their Library.SC differ only in the time of the
# check-out (01/29/83 14:26:49 and 13:58:15): Library_libu8000.SC, Library_slibu8000.SC. compare/libu8000/SYSIII.md.
# Headers: arhdr_libu8000.s, arhdr_slibu8000.s, mkar.sh.
#
#   make        libu8000.a slibu8000.a
#   make install PREFIX=dir   dir/usr/lib/libu8000.a, dir/usr/lib/slibu8000.a
#   make clean

CC = ccv1
SCC = sccv1
CAS = cas
PREFIX = /
CFLAGS =
OBJS = disasm.o

all: build/nonseg/libu8000.a build/seg/slibu8000.a

build/nonseg/libu8000.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CC) -f ../../Makefile libu8000.a

build/seg/slibu8000.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCC) -f ../../Makefile slibu8000.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libu8000.a $(PREFIX)/usr/lib/libu8000.a
	cp build/seg/slibu8000.a $(PREFIX)/usr/lib/slibu8000.a

clean:
	cd build/nonseg ; rm -f libu8000.a  arhdr.o $(OBJS) Library.SC
	cd build/seg    ; rm -f slibu8000.a arhdr.o $(OBJS) Library.SC

##################
# bin targets
##################

libu8000.a: $(OBJS) ../../arhdr_libu8000.s ../../mkar.sh ../../Library_libu8000.SC
	cp ../../Library_libu8000.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libu8000.s
	sh ../../mkar.sh libu8000.a arhdr.o $(OBJS) Library.SC

slibu8000.a: $(OBJS) ../../arhdr_slibu8000.s ../../mkar.sh ../../Library_slibu8000.SC
	cp ../../Library_slibu8000.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibu8000.s
	sh ../../mkar.sh slibu8000.a arhdr.o $(OBJS) Library.SC

###################
# obj targets
###################

disasm.o: ../../disasm.c
	$(CC) $(CFLAGS) -c ../../disasm.c

FORCE:

.PHONY: all install clean
