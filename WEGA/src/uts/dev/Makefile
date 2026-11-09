# uts/dev: the device drivers, library LIB2 (usr/sys/dev/LIB2), and the sources shipped beside it.
# Compiled with the segmented scc. The drivers of the optional devices (mt, rd, udev1 ... udev6) are dummy objects from *.d.s,
# as in the shipped LIB2. koppel.s, disk.s, the dummies: original assembler (as -u); clist.s: assembler for cas (cas -u).
#
#   make                       build LIB2
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and LIB2

AS = as
CAS = cas
SCCVK = sccvk
CC = scc
CFLAGS = -O
AR = ar
PREFIX = /
IDIR = $(PREFIX)/usr/sys/dev

# the order of the members is the order of the shipped LIB2
OBJ = bio.o siok.o koppel.o ud.o fd.o mem.o sys.o tty.o tt0.o partab.o md.o \
	disk.o lp.o rd.o mt.o cpu2.o udev1.o udev2.o udev3.o udev4.o udev5.o \
	udev6.o clist.o dump.o

IFILES = blockdev.c chardev.c lp.c ttydev.c udev1.c udev2.c udev3.c udev4.c \
	udev5.c udev6.c LIB2

all: LIB2
	cd wega ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all

LIB2: $(OBJ)
	rm -f LIB2
	$(AR) cr LIB2 $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $<

# compiled by the code generator of the kernel (sccvk, see ../../../host/OLDCC.md)
tty.o: tty.c
	$(SCCVK) $(CFLAGS) -c tty.c

tt0.o: tt0.c
	$(SCCVK) $(CFLAGS) -c tt0.c

koppel.o: koppel.s
	$(AS) -u -o $@ koppel.s

disk.o: disk.s
	$(AS) -u -o $@ disk.s

# dummy objects
mt.o: mt.d.s
	$(AS) -u -o $@ mt.d.s
rd.o: rd.d.s
	$(AS) -u -o $@ rd.d.s
udev1.o: udev1.d.s
	$(AS) -u -o $@ udev1.d.s
udev2.o: udev2.d.s
	$(AS) -u -o $@ udev2.d.s
udev3.o: udev3.d.s
	$(AS) -u -o $@ udev3.d.s
udev4.o: udev4.d.s
	$(AS) -u -o $@ udev4.d.s
udev5.o: udev5.d.s
	$(AS) -u -o $@ udev5.d.s
udev6.o: udev6.d.s
	$(AS) -u -o $@ udev6.d.s

# C-ASM
clist.o: clist.s
	$(CAS) -u -o $@ clist.s

install: LIB2
	test -d $(PREFIX)/usr || mkdir $(PREFIX)/usr
	test -d $(PREFIX)/usr/sys || mkdir $(PREFIX)/usr/sys
	test -d $(IDIR) || mkdir $(IDIR)
	for f in $(IFILES); do cp $$f $(IDIR)/$$f; done

clean:
	rm -f $(OBJ) LIB2
	cd wega ; $(MAKE) clean

.PHONY: all install clean
