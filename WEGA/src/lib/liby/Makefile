# liby: /lib/liby.a and /lib/sliby.a (the yacc library, nonsegmented and segmented), from the System III sources
# (tmp/sys3/usr/src/lib/liby: libmai.c is main.c, libzer.c is yyerror.c). Both libraries are made from the same sources
# (their Library.SC are equal, 02/10/83) by the compiler of that time, v1 (ccv1, sccv1 from OLDROOT); the archives are
# byte-identical (headers: arhdr_liby.s, arhdr_sliby.s, mkar.sh). WEGA changed main.c: return(yyparse()) (host/KNOWLEDGE.md,
# "liby.a, sliby.a").
#
#   make        liby.a sliby.a
#   make install PREFIX=dir   dir/lib/liby.a, dir/lib/sliby.a
#   make clean

CCV1 = ccv1
SCCV1 = sccv1
CAS = cas
PREFIX = /
CFLAGS = -O
LOBJS = main.o yyerror.o
SOBJS = main.s.o yyerror.s.o

all: build/nonseg/liby.a build/seg/sliby.a

build/nonseg/liby.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CCV1) -f ../../Makefile liby.a

build/seg/sliby.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCCV1) -f ../../Makefile sliby.a

install: all
	cp build/nonseg/liby.a $(PREFIX)/lib/liby.a
	cp build/seg/sliby.a $(PREFIX)/lib/sliby.a

clean:
	-cd build/nonseg ; rm -f liby.a  arhdr.o $(LOBJS) Library.SC
	-cd build/seg    ; rm -f sliby.a arhdr.o $(SOBJS) Library.SC

##################
# bin targets
##################

liby.a: $(LOBJS) ../../arhdr_liby.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_liby.s
	sh ../../mkar.sh liby.a arhdr.o $(LOBJS) Library.SC

sliby.a: $(SOBJS) ../../arhdr_sliby.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_sliby.s
	sh ../../mkar.sh sliby.a arhdr.o $(SOBJS) Library.SC

###################
# obj targets
###################

main.o: ../../main.c
	$(CC) $(CFLAGS) -c ../../main.c
yyerror.o: ../../yyerror.c
	$(CC) $(CFLAGS) -c ../../yyerror.c
main.s.o: ../../main.c
	$(CC) $(CFLAGS) -c ../../main.c
	mv main.o main.s.o
yyerror.s.o: ../../yyerror.c
	$(CC) $(CFLAGS) -c ../../yyerror.c
	mv yyerror.o yyerror.s.o

FORCE:

.PHONY: all install clean
