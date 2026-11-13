# libl: /lib/libl.a and /lib/slibl.a (the lex library, nonsegmented and segmented) and /usr/lib/lex/ncform, from the
# System III sources (tmp/sys3/usr/src/lib/libl). Both libraries are made from the same sources (their Library.SC are equal,
# 02/11/83) by the compiler of that time, v1 (ccv1, sccv1 from OLDROOT); the archives are byte-identical (headers:
# arhdr_libl.s, arhdr_slibl.s, mkar.sh). oldcc/allprint, oldcc/allprint.s: allprint cut from the original members
# (HANDOFF.md).
#
#   make        libl.a slibl.a
#   make install PREFIX=dir   dir/lib/libl.a, dir/lib/slibl.a, dir/usr/lib/lex/ncform
#   make clean

CCV1 = ccv1
SCCV1 = sccv1
CAS = cas
PREFIX = /
CFLAGS = -O
ALLOBJS = main.o reject.o yyless.o yywrap.o
LOBJS = allprint.o $(ALLOBJS)
SOBJS = allprint.s.o $(ALLOBJS)

all: build/nonseg/libl.a build/seg/slibl.a

build/nonseg/libl.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CCV1) -f ../../Makefile libl.a

build/seg/slibl.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCCV1) -f ../../Makefile slibl.a

install: all
	cp build/nonseg/libl.a $(PREFIX)/lib/libl.a
	cp build/seg/slibl.a $(PREFIX)/lib/slibl.a
	-mkdir $(PREFIX)/usr/lib $(PREFIX)/usr/lib/lex
	cp ncform $(PREFIX)/usr/lib/lex/ncform

clean:
	cd build/nonseg ; rm -f libl.a  arhdr.o $(LOBJS) Library.SC
	cd build/seg    ; rm -f slibl.a arhdr.o $(SOBJS) Library.SC

##################
# bin targets
##################

libl.a: $(LOBJS) ../../arhdr_libl.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libl.s
	sh ../../mkar.sh libl.a arhdr.o $(LOBJS) Library.SC

slibl.a: $(SOBJS) ../../arhdr_slibl.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibl.s
	sh ../../mkar.sh slibl.a arhdr.o $(SOBJS) Library.SC

###################
# obj targets
###################

allprint.o: ../../oldcc/allprint/0_1.s
	$(CAS) -u -o allprint.o ../../oldcc/allprint/0_1.s
allprint.s.o: ../../oldcc/allprint.s/0_1.s
	$(CAS) -u -o allprint.s.o ../../oldcc/allprint.s/0_1.s
main.o: ../../main.c
	$(CC) $(CFLAGS) -c ../../main.c
reject.o: ../../reject.c
	$(CC) $(CFLAGS) -c ../../reject.c
yyless.o: ../../yyless.c
	$(CC) $(CFLAGS) -c ../../yyless.c
yywrap.o: ../../yywrap.c
	$(CC) $(CFLAGS) -c ../../yywrap.c

FORCE:

.PHONY: all install clean
