# standalone: the stand-alone programs sa.* with their library libb.a.
# Compiled with the WEGA cc and cas, archived with ar, linked with ld
# (no crt0, no libc: these run without the operating system).
# sa.diags is a separate build (sa.diags/Makefile) and not part of "all".
#
#   make                       build the sa.* programs
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects, libb.a and the programs

AS ?= as   # the original standalone build assembles with as
CC ?= cc
CFLAGS ?= -O
LD ?= ld
AR ?= ar
PREFIX ?= /

FILES = sa.cat sa.format sa.verify sa.mkfs sa.install sa.timer sa.shipdisk
BMISC_C = ../../uts/conf/bmisc.c

SUBDIRS=	sa.diags

all: $(FILES) subdirs

subdirs:
	fail=0; for d in $(SUBDIRS); do $(MAKE) -C $$d all || fail=1; done; exit $$fail

.s.o:
	$(AS) -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c $<

bmisc.o: $(BMISC_C)
	$(CC) $(CFLAGS) -c -o bmisc.o $(BMISC_C)

libb.a: sa.function.o bmisc.o
	rm -f libb.a
	$(AR) qc libb.a sa.function.o bmisc.o
	chmod 644 libb.a

sa.mkfs: sa.mkfs.o libb.a
	$(LD) -s -o $@ sa.mkfs.o libb.a

sa.cat: sa.cat.o libb.a
	$(LD) -s -o $@ sa.cat.o libb.a

sa.timer: sa.timer.o sa.function.o sa.inout.o bmisc.o
	$(LD) -s -o $@ sa.timer.o sa.function.o sa.inout.o bmisc.o

sa.install: sa.install.o libb.a
	$(LD) -s -o $@ sa.install.o libb.a

ft41.o: sa.format.c
	cp sa.format.c ft41.c
	$(CC) -c -o ft41.o ft41.c

sa.format: ft41.o wdc.o libb.a
	$(LD) -o $@ ft41.o wdc.o libb.a

vf41.o: sa.verify.c
	cp sa.verify.c vf41.c
	$(CC) -c -o vf41.o vf41.c

sa.verify: vf41.o wdc.o libb.a
	$(LD) -o $@ vf41.o wdc.o libb.a

sd41.o: sa.shipdisk.c
	cp sa.shipdisk.c sd41.c
	$(CC) -c -o sd41.o sd41.c

sa.shipdisk: sd41.o wdc.o libb.a
	$(LD) -o $@ sd41.o wdc.o libb.a

install: $(FILES)
	for p in $(FILES); do cp $$p $(PREFIX)/$$p; chmod 400 $(PREFIX)/$$p; done

clean:
	rm -f $(FILES) sd41.c ft41.c vf41.c libb.a *.o
	for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done

.PHONY: all subdirs install clean
