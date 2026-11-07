# casold: the assembler of the old C compilers v0 and v0p (host/OLDCC.md, D17). It is the source of .. with -DOLD_CAS:
# the early jump optimization. The passes of v0 and v0p and their driver are in ../../{cparse,codgen,popt,cc}/v0*.
#
#   make                       old/cas (the original tools: SCC, YACC, SLD as for ..)
#   make install PREFIX=dir    dir/lib/old/cas (the assembler of ccv0 and ccv0p)
#   make clean

SCC = scc
YACC = yacc
SLD = sld
SCRT0 = /lib/scrt0.o
SLIBC = /lib/slibc.a
PREFIX = /

all: cas

cas: ../jpr.c ../optim.c ../ssymtab.c ../cas.h
	cp ../assemble.c ../cas.h ../done.c ../error.c ../expr.c ../gen.c ../getops.c ../ifile.c ../instr.c ../itable.c ../jpr2.c ../jpr.c ../listing.c ../locntr.c ../main.c ../nodes.c ../ntable.c ../ofile.c ../optim.c ../p2loop.c ../pstab.c ../pst.c ../pstdata.h ../scanner.c ../ssymtab.c ../stable.c ../subr.c ../symbols.c ../table1.c ../table2.c ../parser.y .
	$(MAKE) -f ../Makefile cas SCC='$(SCC)' SCCFLAGS='-O -c -DOLD_CAS' YACC='$(YACC)' SLD='$(SLD)' SCRT0='$(SCRT0)' SLIBC='$(SLIBC)'

install: cas
	-mkdir $(PREFIX)/lib/old
	cp cas $(PREFIX)/lib/old/cas

clean:
	rm -f *.o *.c *.h parser.y y.tab.c y.tab.h cas

.PHONY: all install clean
