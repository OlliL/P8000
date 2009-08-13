# MAKE-Datei zur Erzeugung des sa.diags

#take from MON16 Firmware:
# p.gesa.s
# p.ldsd.s
# p.term.s


LDFLAGS= -s -b 0x8000 -e _main
OBJECTS= menu.o sa.test.o twem.o wdc.o init.o p.term.o comm.o brk.o p.gesa.o \
boot.o disk.o p.ldsd.o misc.o mch.o ram.o
PROCED= sa.diags

$(PROCED) : $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o $(PROCED)
