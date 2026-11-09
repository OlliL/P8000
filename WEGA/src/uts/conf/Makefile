# uts/conf: the kernel configuration: wega (the kernel, linked from the start-up objects, LIB1 and LIB2), boot (the boot loader from
# bstart.o and BOOTLIB) and the files that sysgen needs in usr/sys/conf. make.wega and make.boot are the shipped files that sysgen
# uses; this Makefile does the same steps.
# ver.c is made from ver.c+ by chkout (the rule .c+.c of the native make); its text contains the date of the checkout.
# u.o is made from u.s by as and mku.sh (the segment of _u); the boot loader is compiled with cc (non-segmented), the kernel part with scc.
#
#   make                       build wega, boot and the objects
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and the results

AS = as
CAS = cas
CC = cc
SCC = scc
CFLAGS = -O
CHKOUT = chkout
LD = ld
SLD = sld
PREFIX = /
IDIR = $(PREFIX)/usr/sys/conf

# the objects of the boot loader, in the order of the shipped BOOTLIB
BL_OBJ = bmch.o boot.o bsys.o bconf.o bmisc.o bmd.o bfd.o bud.o brm.o
# the objects that go to usr/sys/conf and into the link of wega
OBJ = bstart.o conf.o event.o fpe.o mch.o u.o wpar.o
WEGA_OBJ = wpar.o ../dev/wega/event.o mch.o u.o ver.o fpe.o conf.o ../sys/LIB1 ../dev/wega/LIB2W

IFILES = BOOTLIB bstart.c bstart.o conf.o event.o fpe.o make.boot make.wega \
	mch.o u.o ver.c+ wpar.c

all: wega boot event.o wpar30/wpar.o

wega: $(WEGA_OBJ)
	$(SLD) -Ns -o wega -e start -X -i -Ms62 $(WEGA_OBJ)

boot: bstart.o BOOTLIB
	$(LD) -s -e start -b 0x8000 -o boot bstart.o BOOTLIB

# the headers of the shipped archive: arhdr.s, ../mkar.sh
BOOTLIB: $(BL_OBJ) arhdr.s ../mkar.sh
	$(CAS) -u -o arhdr.o arhdr.s
	sh ../mkar.sh BOOTLIB arhdr.o $(BL_OBJ)

# ../sys/LIB1 and ../dev/LIB2 are made by "make" in uts
../sys/LIB1:
	cd ../sys ; $(MAKE) LIB1

../dev/wega/LIB2W:
	cd ../dev/wega ; $(MAKE) LIB2W

../dev/wega/event.o:
	cd ../dev/wega ; $(MAKE) event.o

.c.o:
	$(CC) $(CFLAGS) -c $<

conf.o: conf.c
	$(SCC) $(CFLAGS) -c conf.c

wpar.o: wpar.c ../h/sysparm.h ../h/mdsize.h ../h/ver.h
	$(SCC) $(CFLAGS) -c wpar.c

# the wpar.o installed in usr/sys/conf is older than wpar.c and the wega linked here: its own source and configuration
wpar30/wpar.o: wpar30/wpar.c wpar30/sysparm.h wpar30/mdsize.h wpar30/ver.h
	cd wpar30 ; $(SCC) $(CFLAGS) -c wpar.c

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

# as cannot give _u its segment: mku.sh sets it
u.o: u.s mku.sh
	$(AS) -u -o u1.o u.s
	sh mku.sh u1.o u.o
	rm -f u1.o

install: all
	test -d $(PREFIX)/usr || mkdir $(PREFIX)/usr
	test -d $(PREFIX)/usr/sys || mkdir $(PREFIX)/usr/sys
	test -d $(IDIR) || mkdir $(IDIR)
	for f in $(IFILES); do cp $$f $(IDIR)/$$f; done
	cp wpar30/wpar.o $(IDIR)/wpar.o
	cp boot $(PREFIX)/boot
	cp boot $(PREFIX)/usr/boot
	cp wega $(PREFIX)/wega

clean:
	rm -f $(BL_OBJ) $(OBJ) arhdr.o ver.c ver.o BOOTLIB wega boot wpar30/wpar.o

.PHONY: all install clean
