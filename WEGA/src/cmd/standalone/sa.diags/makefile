# MAKE-Datei zur Erzeugung des sa.diags

LDFLAGS= -s -b 0x8000 -e _main
OBJECTS= menu.o sa.test.o twem.o wdc.o init.o term.o comm.o brk.o gesa.o \
boot.o disk.o ldsd.o misc.o mch.o ram.o
PROCED= sa.diags

$(PROCED) : $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o $(PROCED)
