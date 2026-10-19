# cas: the cas assembler (U8000 assembler), reconstructed from the binary /bin/cas.
#
#   make                 compile the modules that exist
#   make check WEGA=dir  compare them with the original (see check.sh, MODULES)
#   make clean
#
# The link of the whole program (scrt0, the modules in the order of MODULES, slibc.a,
# the table segments 7, 8, 9) is added when all modules exist.

SCC = scc
SCCFLAGS = -c
OBJS = ifile.o nodes.o

all: $(OBJS)

.c.o:
	$(SCC) $(SCCFLAGS) $<

$(OBJS): cas.h

check:
	sh check.sh WEGA=$(WEGA)

clean:
	rm -f *.o

.PHONY: all check clean
