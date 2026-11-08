# uts/sys: the kernel proper, library LIB1 (usr/sys/sys/LIB1).
# Compiled with the segmented scc; lock.o is made from the assembler source lock.s (a disassembly, see ../TODO.md) by cas.
#
#   make                       build LIB1
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and LIB1

CAS = cas
SCCVK = sccvk
CC = scc
CFLAGS = -O
AR = ar
PREFIX = /
IDIR = $(PREFIX)/usr/sys/sys

# the order of the members is the order of the shipped LIB1
OBJ = main.o trap.o sig.o iget.o prf.o slp.o subr.o rdwri.o fio.o malloc.o \
	alloc.o machdep.o nami.o pipe.o sysent.o sys3.o sys1.o sys4.o sys2.o \
	acct.o text.o ureg.o break.o lock.o mkseg.o debug.o timer.o

all: LIB1

LIB1: $(OBJ)
	rm -f LIB1
	$(AR) cr LIB1 $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $<

# compiled by the code generator of the kernel (sccvk, see ../../../host/OLDCC.md)
sig.o: sig.c
	$(SCCVK) $(CFLAGS) -c sig.c

sys2.o: sys2.c
	$(SCCVK) $(CFLAGS) -c sys2.c

sys3.o: sys3.c
	$(SCCVK) $(CFLAGS) -c sys3.c

debug.o: debug.c
	$(SCCVK) $(CFLAGS) -c debug.c

ureg.o: ureg.c
	$(SCCVK) $(CFLAGS) -c ureg.c

lock.o: lock.s
	$(CAS) -u -o $@ lock.s

install: LIB1
	test -d $(PREFIX)/usr || mkdir $(PREFIX)/usr
	test -d $(PREFIX)/usr/sys || mkdir $(PREFIX)/usr/sys
	test -d $(IDIR) || mkdir $(IDIR)
	cp LIB1 $(IDIR)/LIB1

clean:
	rm -f $(OBJ) LIB1

.PHONY: all install clean
