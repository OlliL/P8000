# cparse: pass 1 of cc (PCC front end) of WEGA, reconstructed from /lib/cparse (E711, nonsegmented, stripped).
#
#   make block       step 3 of host/HOWTO_reconstruct.md: the own part of the original as one raw module (made from
#                    the original by tools/rawblock.py, needs WEGA=root of the original tree), linked with the libc part
#                    cplibc.s; the result cparse_block must be equal to /lib/cparse (make checkblock)
#   make             cparse from our sources: crt0-codgen.o, the modules in the order of the original, the libc part
#   make clean
#
# The parser cgram1.s is cut from the original (tools/cutgram.py) until the grammar is rebuilt; the libc part is one
# cut module (cplibc.s) until it is split into the shipped members and the oldlibc versions (tools/HANDOFF_cparse.md).

CRT = ../../lib/libc/crt
CAS = cas
LDBIN = /bin/ld
WEGA = ../../../../tmp/wega31_root

CC = cc
CFLAGS = -O
OBJS = cgram1.o xdefs1.o scan1.o pftn1.o trees1.o optim1.o code1.o local1.o comm1.o vinit1.o sym1.o
HDRS = mfile1 manifest macdefs stdio.h

all: cparse

cparse: $(CRT)/crt0-codgen.o $(OBJS) cplibc.o
	$(LDBIN) -i -e start $(CRT)/crt0-codgen.o $(OBJS) cplibc.o -o $@

$(OBJS): $(HDRS)
optim1.o: optim
comm1.o: common

.c.o:
	$(CC) $(CFLAGS) -c $<

cgram1.o: cgram1.s
	$(CAS) -u -o $@ cgram1.s

block: cparse_block

cparse_block: $(CRT)/crt0-codgen.o ownraw.o cplibc.o
	$(LDBIN) -i -e start $(CRT)/crt0-codgen.o ownraw.o cplibc.o -o $@

ownraw.s: tools/rawblock.py
	WEGA=$(WEGA) python3 tools/rawblock.py $@

ownraw.o: ownraw.s
	$(CAS) -u -o $@ ownraw.s

cplibc.o: cplibc.s
	$(CAS) -u -o $@ cplibc.s

checkblock: cparse_block
	python3 tools/cmpimage.py cparse_block $(WEGA)/lib/cparse

clean:
	rm -f ownraw.s ownraw.o cplibc.o cparse_block cparse *.o
