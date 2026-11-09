# uts/sys: the kernel proper, library LIB1 (usr/sys/sys/LIB1).
# Compiled with the segmented scc.
#
#   make                       build LIB1
#   make install PREFIX=dir    install below dir (default /)
#   make clean                 remove the objects and LIB1

SCCVK = sccvk
CC = scc
CFLAGS = -O
CAS = cas
PREFIX = /
IDIR = $(PREFIX)/usr/sys/sys

# the order of the members is the order of the shipped LIB1
OBJ = main.o trap.o sig.o iget.o prf.o slp.o subr.o rdwri.o fio.o malloc.o \
	alloc.o machdep.o nami.o pipe.o sysent.o sys3.o sys1.o sys4.o sys2.o \
	acct.o text.o ureg.o break.o lock.o mkseg.o debug.o timer.o

all: LIB1

# the headers of the shipped archive: arhdr.s, ../mkar.sh
LIB1: $(OBJ) arhdr.s ../mkar.sh
	$(CAS) -u -o arhdr.o arhdr.s
	sh ../mkar.sh LIB1 arhdr.o $(OBJ)

.c.o:
	$(CC) $(CFLAGS) -c $<

# compiled by the code generator of the kernel (sccvk, see ../../../host/OLDCC.md)
sys1.o: sys1.c
	$(SCCVK) $(CFLAGS) -c sys1.c

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

lock.o: lock.c
	$(SCCVK) $(CFLAGS) -c lock.c

install: LIB1
	test -d $(PREFIX)/usr || mkdir $(PREFIX)/usr
	test -d $(PREFIX)/usr/sys || mkdir $(PREFIX)/usr/sys
	test -d $(IDIR) || mkdir $(IDIR)
	cp LIB1 $(IDIR)/LIB1

clean:
	rm -f $(OBJ) arhdr.o LIB1

.PHONY: all install clean
