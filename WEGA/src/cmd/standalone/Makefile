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

all: $(FILES)

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
	chmod 400 $@

sa.cat: sa.cat.o libb.a
	$(LD) -s -o $@ sa.cat.o libb.a
	chmod 400 $@

sa.timer: sa.timer.o libb.a
	$(LD) -s -o $@ sa.timer.o libb.a
	chmod 400 $@

sa.install: sa.install.o libb.a
	$(LD) -s -o $@ sa.install.o libb.a
	chmod 400 $@

sa.format: sa.format.o sa.wdc.o libb.a
	$(LD) -s -o $@ sa.format.o sa.wdc.o libb.a
	chmod 400 $@

sa.verify: sa.verify.o sa.wdc.o libb.a
	$(LD) -s -o $@ sa.verify.o sa.wdc.o libb.a
	chmod 400 $@

sa.shipdisk: sa.shipdisk.o sa.wdc.o libb.a
	$(LD) -s -o $@ sa.shipdisk.o sa.wdc.o libb.a
	chmod 400 $@

install: $(FILES)
	for p in $(FILES); do cp $$p $(PREFIX)/$$p; done

clean:
	rm -f $(FILES) libb.a *.o

.PHONY: all install clean
