
CAS=	cas
AS=	as
CC=	scc
CFLAGS= -O


OBJ=	bio.o siok.o koppel.o ud.o fd.o mem.o sys.o tty.o tt0.o partab.o md.o \
	disk.o lp.o rd.o mt.o cpu2.o udev1.o udev2.o udev3.o udev4.o udev5.o \
	udev6.o clist.o dump.o

IFILES=	blockdev.c chardev.c lp.c ttydev.c udev1.c udev2.c udev3.c udev4.c\
	udev5.c udev6.c LIB2
IDIR=	/usr/sys/dev

OWN=	bin
GRP=	system
MOD=	644

all:	LIB2

install: LIB2
	sh -c 'test -d $(IDIR) || mkdir $(IDIR)'
	sh -c 'for file in $(IFILES) ; do \
		cp $$file $(IDIR) ; \
		chmog $(MOD) $(OWN) $(GRP) $(IDIR)/$$file ; \
	done'

LIB2:	$(OBJ)
	chmog 666 26 1 koppel.o
	chmog 666 26 1 fd.o
	chmog 644 89 0 tty.o
	chmog 666 26 1 md.o
	chmog 666 26 1 disk.o
	ar cr LIB2 $(OBJ)

# use dummy objects
mt.o:
	$(AS) -u -o $@ $*.d.s
rd.o:
	$(AS) -u -o $@ $*.d.s
udev1.o:
	$(AS) -u -o $@ $*.d.s
udev2.o:
	$(AS) -u -o $@ $*.d.s
udev3.o:
	$(AS) -u -o $@ $*.d.s
udev4.o:
	$(AS) -u -o $@ $*.d.s
udev5.o:
	$(AS) -u -o $@ $*.d.s
udev6.o:
	$(AS) -u -o $@ $*.d.s

# this files are written in plain ASM
clist.o:
	$(CAS) -u -o $@ $*.s

clean:
	rm -f $(OBJ) LIB2
