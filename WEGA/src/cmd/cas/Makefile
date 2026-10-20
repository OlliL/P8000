# cas: the cas assembler (U8000 assembler), reconstructed from the binary /bin/cas.
#
#   make                 compile the modules that exist
#   make check WEGA=dir  compare them with the original (see check.sh, MODULES)
#   make clean
#
# The link of the whole program (scrt0, the modules in the order of MODULES, slibc.a,
# the table segments 7, 8, 9) is added when all modules exist.

SCC = scc
SCCFLAGS = -O -c
YACC = yacc
OBJS = ifile.o instr.o scanner.o main.o done.o nodes.o parser.o pst.o subr.o symbols.o ssymtab.o locntr.o table1.o table2.o pstab.o

all: $(OBJS)

.c.o:
	$(SCC) $(SCCFLAGS) $<

$(OBJS): cas.h

parser.c: parser.y
	$(YACC) -d parser.y
	mv y.tab.c parser.c

y.tab.h: parser.c

scanner.o: y.tab.h

parser.o: parser.c
	$(SCC) -c parser.c

check:
	sh check.sh WEGA=$(WEGA)

clean:
	rm -f *.o parser.c y.tab.c y.tab.h

.PHONY: all check clean
