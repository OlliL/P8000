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
# Stack segment: the original has no -z flag (relflg 0x01), no table entry for the stack and _stkseg = 63. The shipped sld gives
# _stkseg = maxseg without a table entry only with at most one -M option (stksegn -3 in middle() of src/cmd/sld/ldm.c); -Ms63 gives 63
# with a table entry, -z 127 and the flag 0x21. So gopt is linked by sldo (src/cmd/sld: make old, an old sld whose stack segment of
# -Ms without contents gets no table entry), installed to OLDROOT/bin/sldo by stage2 of src/Makefile.
# The compiler is sccvc by measurement (compare/gopt/findings.json), not the one of the date (v1).

SCC = sccvc
CFLAGS = -O -I../../../head/oldv7
CAS = cas
SCRT0 = /lib/scrt0.o
AR = ar
SLD = sldo
SLDFLAGS = -i -e start -Md7 -Mb8 -Ms63
LIBC = ../../../lib/slibc-v12.a
PREFIX = /
STRIP = strip

# link order of the original (data order)
MODULES = reader2 quads2 debug2 rdr2 comm2 wrtil2 alias2 flow2 dom2 init2 live2 ebbo2 lpinv2 lpind2 beta2 reg2 alias2b
OBJS4 = gdata.o reader2.o quads2.o debug2.o rdr2.o comm2.o wrtil2.o alias2.o flow2.o dom2.o init2.o live2.o
OBJS5 = ebbo2.o lpinv2.o lpind2.o
OBJS4B = beta2.o
OBJS5B = reg2.o alias2b.o
OBJS = $(OBJS4) $(OBJS5) $(OBJS4B) $(OBJS5B)
# the library members of segment 4 (see the gopt rule)
LIBC4 = fseek.o ftell.o rdwr.o fopen.o getw.o
HEADERS = gopt.h ops_gopt.c

all: gopt

# The start-up code is the one of the library (/lib/scrt0.o, Rev 3.2: it saves rr8 first), not a crt0.list template.
# The first five members of the library (fseek ftell rdwr fopen getw) fill the end of segment 4 in the original (from 4:fc3a
# to 4:fffe), the others follow the modules in segment 5: they are linked by name with -Mt4 (a whole library behind -Mt4
# puts what does not fit into segment 0). sldo warns that the archive defines their symbols again.

gopt: $(OBJS) $(LIBC4) $(LIBC)
	$(SLD) $(SLDFLAGS) -Mt4 $(SCRT0) $(OBJS4) -Mt5 $(OBJS5) -Mt4 $(OBJS4B) -Mt5 $(OBJS5B) -Mt4 $(LIBC4) -Mt5 $(LIBC) -o $@

$(LIBC4): $(LIBC)
	$(AR) x $(LIBC) $(LIBC4)

# make shipped (src/cmd/Makefile) links with LIBC=/lib/slibc.a of the guest: no rule for it here
/lib/libc.a /lib/slibc.a:

part: comm2.o rdr2.o reader2.o quads2.o debug2.o wrtil2.o alias2.o flow2.o dom2.o init2.o live2.o ebbo2.o

install: gopt
	cp gopt $(PREFIX)/lib/gopt
	$(STRIP) $(PREFIX)/lib/gopt

reader2.o: reader2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c reader2.c
quads2.o: quads2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c quads2.c
debug2.o: debug2.c $(HEADERS)
	$(SCC) $(CFLAGS) -c debug2.c
rdr2.o: rdr2.c rdr.2.4 $(HEADERS)
	$(SCC) $(CFLAGS) -c rdr2.c
comm2.o: comm2.c common.2.4 $(HEADERS)
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
gdata.o: gdata.c
	$(SCC) $(CFLAGS) -c gdata.c
alias2b.o: alias2b.c $(HEADERS)
	$(SCC) $(CFLAGS) -c alias2b.c

clean:
	rm -f $(OBJS) $(LIBC4) gopt
