# cas: the WEGA U8000 assembler built for Linux (a cross tool: it behaves like /bin/cas of WEGA
# and writes the same object files). Source: derived from WEGA/src/cmd/cas (the decompilation of
# /bin/cas); table1.c, table2.c, pstab.c and pstdata.h are the generated tables of that directory.
#
#   make                 build ./cas
#   make parser          make parser.c and parser.h from parser.y with the original yacc
#                        (run by z8k-emu, WEGA=...). Both files are in the repository: the
#                        tables of another yacc handle syntax errors differently.

HOSTCC = cc
CFLAGS = -O2 -g -Wall -Wno-unused -Wno-implicit-fallthrough -funsigned-char
WEGA = ../../../tmp/wega31_root
EMU  = $(abspath ../z8k-emu/z8k-emu)

OBJS = main.o error.o ifile.o getops.o instr.o scanner.o nodes.o parser.o pst.o subr.o symbols.o \
	ssymtab.o table1.o table2.o locntr.o expr.o p2loop.o ofile.o gen.o assemble.o pstab.o \
	jpr.o jpr2.o optim.o listing.o

all: cas casold

cas: $(OBJS)
	$(HOSTCC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c cas.h
	$(HOSTCC) $(CFLAGS) -c -o $@ $<

# parser.c is kept in the repository (mkparser.sh): no rule of make may overwrite it with bison
%.c: %.y

# casold: the early jump optimization of the old cas (../OLDCC.md, D17)
OLDOBJS = $(filter-out jpr.o ssymtab.o optim.o,$(OBJS)) jpr_old.o ssymtab_old.o optim_old.o

casold: $(OLDOBJS)
	$(HOSTCC) $(CFLAGS) -o $@ $(OLDOBJS)

%_old.o: %.c cas.h
	$(HOSTCC) $(CFLAGS) -DOLD_CAS -c -o $@ $<

parser:
	WEGA=$(abspath $(WEGA)) EMU=$(EMU) sh mkparser.sh

scanner.o: parser.h

clean:
	rm -f *.o cas

.PHONY: all clean parser
