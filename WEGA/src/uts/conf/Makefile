# uts/conf: the kernel configuration: wega (the kernel, linked from the start-up objects, LIB1 and LIB2), boot (the boot loader from
# bstart.o and BOOTLIB) and the files that sysgen needs in usr/sys/conf. make.wega and make.boot are the shipped files that sysgen
# uses; this Makefile does the same steps.
# ver.c is made from ver.c+ by chkout (the rule .c+.c of the native make); its text contains the date of the checkout.
# u.o is the shipped object u.obj (see TODO.md); the boot loader is compiled with cc (non-segmented), the kernel part with scc.
#
#   make                       build wega, boot and the objects
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the results

AS = as
CC = cc
SCC = scc
CFLAGS = -O
CHKOUT = chkout
AR = ar
LD = ld
SLD = sld
PREFIX = /
IDIR = $(PREFIX)/usr/sys/conf

# the objects of the boot loader, in the order of the shipped BOOTLIB
BL_OBJ = bmch.o boot.o bsys.o bconf.o bmisc.o bmd.o bfd.o bud.o brm.o
# the objects that go to usr/sys/conf and into the link of wega
OBJ = bstart.o conf.o event.o fpe.o mch.o u.o wpar.o
WEGA_OBJ = wpar.o event.o mch.o u.o ver.o fpe.o conf.o ../sys/LIB1 ../dev/LIB2

IFILES = BOOTLIB bstart.c bstart.o conf.o event.o fpe.o make.boot make.wega \
	mch.o u.o ver.c+ wpar.c wpar.o

all: wega boot

wega: $(WEGA_OBJ)
	$(SLD) -Ns -o wega -e start -X -i -Ms62 $(WEGA_OBJ)

boot: bstart.o BOOTLIB
	$(LD) -s -e start -b 0x8000 -o boot bstart.o BOOTLIB

BOOTLIB: $(BL_OBJ)
	rm -f BOOTLIB
	$(AR) qc BOOTLIB $(BL_OBJ)

# ../sys/LIB1 and ../dev/LIB2 are made by "make" in uts
../sys/LIB1:
	cd ../sys ; $(MAKE) LIB1

../dev/LIB2:
	cd ../dev ; $(MAKE) LIB2

.c.o:
	$(CC) $(CFLAGS) -c $<

conf.o: conf.c
	$(SCC) $(CFLAGS) -c conf.c

wpar.o: wpar.c ../h/sysparm.h ../h/mdsize.h ../h/ver.h
	$(SCC) $(CFLAGS) -c wpar.c

ver.c: ver.c+
	$(CHKOUT) ver.c+

ver.o: ver.c
	$(SCC) -c ver.c

# the assembler sources (original PLZ/ASM)
bmch.o: bmch.s
	$(AS) -u -o $@ bmch.s
brm.o: brm.s
	$(AS) -u -o $@ brm.s
bud.o: bud.s
	$(AS) -u -o $@ bud.s
event.o: event.s
	$(AS) -u -o $@ event.s
fpe.o: fpe.s
	$(AS) -u -o $@ fpe.s
mch.o: mch.s
	$(AS) -u -o $@ mch.s

u.o: u.obj
	cp u.obj u.o

install: all
	test -d $(PREFIX)/usr || mkdir $(PREFIX)/usr
	test -d $(PREFIX)/usr/sys || mkdir $(PREFIX)/usr/sys
	test -d $(IDIR) || mkdir $(IDIR)
	for f in $(IFILES); do cp $$f $(IDIR)/$$f; done
	cp boot $(PREFIX)/boot
	cp wega $(PREFIX)/wega

clean:
	rm -f $(BL_OBJ) $(OBJ) ver.c ver.o BOOTLIB wega boot

.PHONY: all install clean
