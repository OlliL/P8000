
CAS=	cas
AS=	as
CC=	scc
CFLAGS= -O

OBJ=	bio.o clist.o cpu2.o disk.o dump.o fd.o koppel.o lp.o md.o mem.o \
	mt.o partab.o rd.o siok.o sys.o tt0.o tt0.o tty.o tty.o ud.o udev1.o \
	udev2.o udev3.o udev4.o udev5.o udev6.o

all:	LIB2

LIB2:	$(OBJ)
	ar cr LIB2 `lorder $(OBJ) | tsort`

clist.o:
	$(CAS) -u -o clist.o clist.s

mt.o:
	$(AS) -u -o mt.o mt.d.s

rd.o:
	$(AS) -u -o rd.o rd.d.s

udev1.o:
	$(AS) -u -o udev1.o udev1.d.s

udev2.o:
	$(AS) -u -o udev2.o udev2.d.s

udev3.o:
	$(AS) -u -o udev3.o udev3.d.s

udev4.o:
	$(AS) -u -o udev4.o udev4.d.s

udev5.o:
	$(AS) -u -o udev5.o udev5.d.s

udev6.o:
	$(AS) -u -o udev6.o udev6.d.s

# use the ASM sources to gain 100% compatibility
tt0.o:
	$(CAS) -u -o tt0.o tt0.s

tty.o:
	$(CAS) -u -o tty.o tty.s

