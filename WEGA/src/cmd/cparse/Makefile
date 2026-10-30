# cparse: pass 1 of cc (PCC front end) of WEGA, reconstructed from /lib/cparse (E711, nonsegmented, stripped).
#
#   make block       step 3 of host/HOWTO_reconstruct.md: the own part of the original as one raw module (made from
#                    the original by tools/rawblock.py, needs WEGA=root of the original tree), linked with the libc part
#                    cplibc.s; the result cparse_block must be equal to /lib/cparse (make checkblock)
#   make clean
#
# The C sources of cparse come later (tools/HANDOFF_cparse.md). The libc part is one cut module (cplibc.s) until it is
# split into the shipped members and the oldlibc versions.

CRT = ../../lib/libc/crt
CAS = cas
LDBIN = /bin/ld
WEGA = ../../../../tmp/wega31_root

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
	rm -f ownraw.s ownraw.o cplibc.o cparse_block *.o
