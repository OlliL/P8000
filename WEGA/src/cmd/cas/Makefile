# cas: the cas assembler (U8000 assembler), reconstructed from the binary /bin/cas.
#
#   make		 build/cas
#   make install PREFIX=dir
#   make clean
#   make oldcas	  old/cas: the cas of the old compilers v0 and v0p, -DOLD_CAS (the early jump optimization)
#   make oldinstall PREFIX=dir  dir/lib/old/cas
#
# The modules are linked in the order of MODULES. As in /bin/cas, text goes to segment 4, data
# and bss to segment 5 and the three tables of the symbol table (stable, itable, ntable) to
# the segments 7, 8 and 9.

# the compiler nearest to /bin/cas (sccvc, host/OLDCC.md); old/cas, built before sccvc exists, by the shipped scc
SCC = sccvc
SCCFLAGS = -O
YACC = yacc
SLD = sld
SLDFLAGS = -i -e start -z -Mt4 -Md5 -Mb5
SCRT0 = /lib/scrt0.o
SLIBC = /lib/slibc.a
PREFIX = /
STRIP = strip
ALL_OBJS = ifile.o instr.o scanner.o main.o done.o nodes.o parser.o pst.o subr.o symbols.o ssymtab.o table1.o table2.o locntr.o expr.o p2loop.o ofile.o gen.o assemble.o getops.o error.o pstab.o jpr.o jpr2.o optim.o listing.o

all: build/cas

oldcas: old/cas

build/cas: FORCE
	cd build ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../Makefile cas

old/cas: FORCE
	cd old ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../Makefile SCC=scc SCCFLAGS="-O -DOLD_CAS" cas

install: build/cas
	cp build/cas $(PREFIX)/bin/cas
	$(STRIP) $(PREFIX)/bin/cas

oldinstall: old/cas
	[ -d $(PREFIX)/lib/old ] ||  mkdir $(PREFIX)/lib/old
	cp old/cas $(PREFIX)/lib/old/cas

clean:
	for dir in build old ; do\
		FILES="$$dir/cas $$dir/parser.c $$dir/y.tab.c $$dir/y.tab.h";\
		for f in $(ALL_OBJS) stable.o itable.o ntable.o; do FILES="$$FILES $$dir/$$f"; done;\
		rm -f $$FILES;\
	done

##################
# bin targets
##################

cas: $(ALL_OBJS) stable.o itable.o ntable.o
	$(SLD) $(SLDFLAGS) $(SCRT0) $(ALL_OBJS) -Md7 stable.o -Md8 itable.o -Md9 ntable.o -Md5 $(SLIBC) -o cas

###################
# obj targets
###################

$(ALL_OBJS) stable.o itable.o ntable.o: ../cas.h
pst.o: ../pstdata.h

y.tab.h: parser.c

parser.c: ../parser.y
	$(YACC) -d ../parser.y
	mv y.tab.c parser.c

scanner.o: y.tab.h

parser.o: parser.c
	$(SCC) -c -I../ parser.c
assemble.o: ../assemble.c
	$(SCC) -c ../assemble.c

stable.o: ../stable.c
	$(SCC) $(SCCFLAGS) -c ../stable.c
itable.o: ../itable.c
	$(SCC) $(SCCFLAGS) -c ../itable.c
ntable.o: ../ntable.c
	$(SCC) $(SCCFLAGS) -c ../ntable.c

ifile.o: ../ifile.c
	$(SCC) $(SCCFLAGS) -c ../ifile.c
instr.o: ../instr.c
	$(SCC) $(SCCFLAGS) -c ../instr.c
scanner.o: ../scanner.c
	$(SCC) $(SCCFLAGS) -I. -c ../scanner.c
main.o: ../main.c
	$(SCC) $(SCCFLAGS) -c ../main.c
done.o: ../done.c
	$(SCC) $(SCCFLAGS) -c ../done.c
nodes.o: ../nodes.c
	$(SCC) $(SCCFLAGS) -c ../nodes.c
pst.o: ../pst.c
	$(SCC) $(SCCFLAGS) -c ../pst.c
subr.o: ../subr.c
	$(SCC) $(SCCFLAGS) -c ../subr.c
symbols.o: ../symbols.c
	$(SCC) $(SCCFLAGS) -c ../symbols.c
ssymtab.o: ../ssymtab.c
	$(SCC) $(SCCFLAGS) -c ../ssymtab.c
table1.o: ../table1.c
	$(SCC) $(SCCFLAGS) -c ../table1.c
table2.o: ../table2.c
	$(SCC) $(SCCFLAGS) -c ../table2.c
locntr.o: ../locntr.c
	$(SCC) $(SCCFLAGS) -c ../locntr.c
expr.o: ../expr.c
	$(SCC) $(SCCFLAGS) -c ../expr.c
p2loop.o: ../p2loop.c
	$(SCC) $(SCCFLAGS) -c ../p2loop.c
ofile.o: ../ofile.c
	$(SCC) $(SCCFLAGS) -c ../ofile.c
gen.o: ../gen.c
	$(SCC) $(SCCFLAGS) -c ../gen.c
getops.o: ../getops.c
	$(SCC) $(SCCFLAGS) -c ../getops.c
error.o: ../error.c
	$(SCC) $(SCCFLAGS) -c ../error.c
pstab.o: ../pstab.c
	$(SCC) $(SCCFLAGS) -c ../pstab.c
jpr.o: ../jpr.c
	$(SCC) $(SCCFLAGS) -c ../jpr.c
jpr2.o: ../jpr2.c
	$(SCC) $(SCCFLAGS) -c ../jpr2.c
optim.o: ../optim.c
	$(SCC) $(SCCFLAGS) -c ../optim.c
listing.o: ../listing.c
	$(SCC) $(SCCFLAGS) -c ../listing.c

FORCE:

.PHONY: all oldcas install oldinstall clean
