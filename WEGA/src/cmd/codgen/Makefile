# codgen, scodgen: the code generators (second pass of cc and scc), reconstructed from /lib/codgen and /lib/scodgen. The same sources
# make both: scodgen is codgen compiled with -DSEG and the tables table3_scodgen.c, ops_scodgen.c, hop_scodgen.c
# Both are non-segmented programs
#
#   make              codgen scodgen
#   make old          the passes of the old compilers in build/nonsegVER, build/segv1 (the switches: in the rules)
#   make install PREFIX=dir, make oldinstall PREFIX=dir (dir/lib/VER/)
#   make clean

CC = ccv0p
CAS = cas
LD = ld
CFLAGS = -O
PREFIX = /
LIBC = ../../../../lib/libc-v5.a
CRT = ../../lib/libc/crt

NONSEG_OBJS = bss3.o reader3.o rdr3.o local3.o order3.o match3.o allo3.o regallo3.o comm3.o table3_codgen.o wrt3.o regs3.o lcl3.o tr3.o optim3.o
SEG_OBJS = bss3.o reader3.o rdr3.o local3.o order3.o match3.o allo3.o regallo3.o comm3.o table3_scodgen.o wrt3.o regs3.o lcl3.o tr3.o optim3.o
ALL_TARGETS = build/nonseg/codgen build/seg/scodgen
OLD_TARGETS = build/nonsegv0/codgen build/nonsegv0p/codgen build/nonsegv1/codgen build/nonsegv1q/codgen build/nonsegv1r/codgen build/segv1/scodgen

all: $(ALL_TARGETS)

old: $(OLD_TARGETS)

build/nonseg/codgen: crt0.o FORCE
	cd build/nonseg    ; $(MAKE) -f ../../Makefile codgen
build/nonsegv0/codgen: crt0.o FORCE
	cd build/nonsegv0  ; $(MAKE) -f ../../Makefile codgen CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V0"
build/nonsegv0p/codgen: crt0.o FORCE
	cd build/nonsegv0p ; $(MAKE) -f ../../Makefile codgen CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V0P"
build/nonsegv1/codgen: crt0.o FORCE
	cd build/nonsegv1  ; $(MAKE) -f ../../Makefile codgen CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_D3"
build/nonsegv1q/codgen: crt0.o FORCE
	cd build/nonsegv1q ; $(MAKE) -f ../../Makefile codgen CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_D3 -DOLD_D14Q"
build/nonsegv1r/codgen: crt0.o FORCE
	cd build/nonsegv1r ; $(MAKE) -f ../../Makefile codgen CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_D3 -DOLD_D14Q -DOLD_D1R"

build/seg/scodgen: crt0.o FORCE
	cd build/seg   ; $(MAKE) -f ../../Makefile scodgen CFLAGS="$(CFLAGS) -DSEG"
build/segv1/scodgen: crt0.o FORCE
	cd build/segv1 ; $(MAKE) -f ../../Makefile scodgen CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DSEG -DOLD_D3"

clean:
	for d in $(ALL_TARGETS) $(OLD_TARGETS) ; do\
		dir=`dirname $$d`;\
		FILES="$$dir/codgen $$dir/scodgen";\
		for f in $(NONSEG_OBJS) $(SEG_OBJS); do FILES="$$FILES $$dir/$$f"; done;\
		rm -f $$FILES;\
	done
	rm -f crt0.o crt0.s

install: $(ALL_TARGETS)
	cp build/nonseg/codgen $(PREFIX)/lib/codgen
	cp build/seg/scodgen   $(PREFIX)/lib/scodgen

oldinstall: $(OLD_TARGETS)
	for v in v0 v0p v1 v1q v1r; do \
		[ -d $(PREFIX)/lib/$$v ] || mkdir $(PREFIX)/lib/$$v ; \
		cp build/nonseg$$v/codgen $(PREFIX)/lib/$$v/codgen ; \
	done
	cp build/segv1/scodgen $(PREFIX)/lib/v1/scodgen

#################
# CRT0 Target
##################
crt0.s: $(CRT)/mkcrt0.sh $(CRT)/crt0.s.in $(CRT)/crt0.list
	sh $(CRT)/mkcrt0.sh codgen > $@

crt0.o: crt0.s
	$(CAS) -u -o $@ $<

##################
# bin targets
##################

codgen: $(NONSEG_OBJS) ../../crt0.o table3_codgen.o
	$(LD) -X -e start -i -o $@ ../../crt0.o $(NONSEG_OBJS) $(LIBC)

scodgen: $(SEG_OBJS) ../../crt0.o table3_scodgen.o
	$(LD) -X -e start -i -o $@ ../../crt0.o $(SEG_OBJS)    $(LIBC)

###################
# obj targets
###################

$(NONSEG_OBJS) $(SEG_OBJS): ../../cg.h ../../cg2.h
local3.o: ../../hop_codgen.c ../../hop_scodgen.c
comm3.o: ../../ops_codgen.c ../../ops_scodgen.c

bss3.o: ../../bss3.c
	$(CC) $(CFLAGS) -c ../../bss3.c
reader3.o: ../../reader3.c
	$(CC) $(CFLAGS) -c ../../reader3.c
rdr3.o: ../../rdr3.c
	$(CC) $(CFLAGS) -c ../../rdr3.c
local3.o: ../../local3.c
	$(CC) $(CFLAGS) -c ../../local3.c
order3.o: ../../order3.c
	$(CC) $(CFLAGS) -c ../../order3.c
match3.o: ../../match3.c
	$(CC) $(CFLAGS) -c ../../match3.c
allo3.o: ../../allo3.c
	$(CC) $(CFLAGS) -c ../../allo3.c
regallo3.o: ../../regallo3.c
	$(CC) $(CFLAGS) -c ../../regallo3.c
comm3.o: ../../comm3.c
	$(CC) $(CFLAGS) -c ../../comm3.c
wrt3.o: ../../wrt3.c
	$(CC) $(CFLAGS) -c ../../wrt3.c
regs3.o: ../../regs3.c
	$(CC) $(CFLAGS) -c ../../regs3.c
lcl3.o: ../../lcl3.c
	$(CC) $(CFLAGS) -c ../../lcl3.c
tr3.o: ../../tr3.c
	$(CC) $(CFLAGS) -c ../../tr3.c
optim3.o: ../../optim3.c
	$(CC) $(CFLAGS) -c ../../optim3.c
table3_codgen.o: ../../table3_codgen.c
	$(CC) $(CFLAGS) -c ../../table3_codgen.c
table3_scodgen.o: ../../table3_scodgen.c
	$(CC) $(CFLAGS) -c ../../table3_scodgen.c

FORCE:

.PHONY: all install clean old oldinstall
