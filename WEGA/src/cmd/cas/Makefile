# cas: the cas assembler (U8000 assembler), reconstructed from the binary /bin/cas.
#
#   make                 build cas
#   make check WEGA=dir  compare the modules with the original (see check.sh, MODULES)
#   make install PREFIX=dir
#   make clean
#
# The modules are linked in the order of MODULES. As in /bin/cas, text goes to segment 4, data
# and bss to segment 5 and the three tables of the symbol table (stable, itable, ntable) to
# the segments 7, 8 and 9.

SCC = scc
SCCFLAGS = -O -c
YACC = yacc
SLD = sld
SLDFLAGS = -i -s -e start -z -Mt4 -Md5 -Mb5
SCRT0 = /lib/scrt0.o
SLIBC = /lib/slibc.a
PREFIX = /
OBJS = ifile.o instr.o scanner.o main.o done.o nodes.o parser.o pst.o subr.o symbols.o ssymtab.o table1.o table2.o locntr.o expr.o p2loop.o ofile.o gen.o assemble.o getops.o error.o pstab.o jpr.o jpr2.o optim.o listing.o

all: cas

cas: $(OBJS) stable.o itable.o ntable.o
	$(SLD) $(SLDFLAGS) $(SCRT0) $(OBJS) -Md7 stable.o -Md8 itable.o -Md9 ntable.o -Md5 $(SLIBC) -o cas

.c.o:
	$(SCC) $(SCCFLAGS) $<

$(OBJS) stable.o itable.o ntable.o: cas.h

parser.c: parser.y
	$(YACC) -d parser.y
	mv y.tab.c parser.c

y.tab.h: parser.c

scanner.o: y.tab.h

parser.o: parser.c
	$(SCC) -c parser.c

assemble.o: assemble.c
	$(SCC) -c assemble.c

check:
	sh check.sh WEGA=$(WEGA)

install: cas
	cp cas $(PREFIX)/bin/cas

clean:
	rm -f *.o cas parser.c y.tab.c y.tab.h

.PHONY: all check install clean
