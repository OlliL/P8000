# gopt: the global optimizer of the C compiler (/lib/gopt), reconstructed from /lib/gopt (no source known).
#
#   make part        the modules that exist
#   make gopt        gopt, linked (not complete yet: reader2, quads2, debug2, wrtil2 ... are not written)
#   make install PREFIX=dir
#   make clean
#
# The modules are linked in the order of the SCCS lines of the data of /lib/gopt (the order of MODULES in compare/gopt/progcfg.py), behind
# the start-up code of the system without its jump and text (crt0.list: gopt). The code is 115966 bytes: more than one segment (0xFFFE bytes)
# holds. The shipped sld puts a text that does not fit into the lowest free segment number (measured: 0, not 5), so the second code
# segment is named: -Mt4 for the first modules, -Mt5 from ebbo2 on (the first module that starts segment 5 in the original: anchored by
# its strings). A later small module still goes by first fit into the free space of segment 4 when -Mt4 is given again (measured with
# synthetic modules). Data in segment 7, bss in segment 8, as in the original (E611, flag 0x20: stack segment 127, entry 04000000).
# Not decided until the link is complete: where the limit between the groups lies (T5FIRST), where the libc members go (the original has
# fseek, ftell, fread, getw at the end of segment 4, printf and the rest in segment 5), the libc revision (LIBC).
# The compiler is sccvc by measurement (compare/gopt/findings.json), not the one of the date (v1).

SCC = sccvc
CFLAGS = -O -I../../head/oldv7
CAS = cas
SLD = sld
SLDFLAGS = -i -e start -z -Md7 -Mb8
CRT = ../../lib/libc/nonseg/crt
LIBC = ../../lib/slibc-v11.a
PREFIX = /
STRIP = strip

# link order of the original (data order)
MODULES = reader2 quads2 debug2 rdr2 comm2 wrtil2 alias2 flow2 dom2 init2 live2 ebbo2 lpinv2 lpind2 beta2 reg2 alias2b
OBJS4 = reader2.o quads2.o debug2.o rdr2.o comm2.o wrtil2.o alias2.o flow2.o dom2.o init2.o live2.o
OBJS5 = ebbo2.o lpinv2.o lpind2.o beta2.o reg2.o alias2b.o
OBJS = $(OBJS4) $(OBJS5)
HEADERS = gopt.h ops_gopt.c

all: gopt

gopt: scrt0.o $(OBJS) $(LIBC)
	$(SLD) $(SLDFLAGS) -Mt4 scrt0.o $(OBJS4) -Mt5 $(OBJS5) $(LIBC) -o $@

part: comm2.o rdr2.o reader2.o quads2.o debug2.o wrtil2.o alias2.o

install: gopt
	cp gopt $(PREFIX)/lib/gopt
	$(STRIP) $(PREFIX)/lib/gopt

scrt0.s: $(CRT)/mkcrt0.sh $(CRT)/scrt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh gopt > $@

scrt0.o: scrt0.s
	$(CAS) -u -o $@ scrt0.s

reader2.o: reader2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c reader2.c
quads2.o: quads2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c quads2.c
debug2.o: debug2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c debug2.c
rdr2.o: rdr2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c rdr2.c
comm2.o: comm2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c comm2.c
wrtil2.o: wrtil2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c wrtil2.c
alias2.o: alias2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c alias2.c
flow2.o: flow2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c flow2.c
dom2.o: dom2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c dom2.c
init2.o: init2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c init2.c
live2.o: live2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c live2.c
ebbo2.o: ebbo2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c ebbo2.c
lpinv2.o: lpinv2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c lpinv2.c
lpind2.o: lpind2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c lpind2.c
beta2.o: beta2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c beta2.c
reg2.o: reg2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c reg2.c
alias2b.o: alias2b.c $(HEADERS)
	$(SCC) $(CFLAGS) -c alias2b.c

clean:
	rm -f *.o scrt0.s gopt
