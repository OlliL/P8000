# sa.diags: diagnostic program extended with WDOS Test, meant for
# the P8000 compact (but got never released)

AS = as   # the original standalone build assembles with as
CC = cc
CFLAGS = -O
LD = ld
LDFLAGS = -s -b 0x8000 -e _main
PREFIX = /

OBJECTS = menu.o sa.test.o twem.o wdc.o init.o p.term.o comm.o brk.o \
          p.gesa.o boot.o disk.o p.ldsd.o misc.o mch.o ram.o
PROCED = sa.diags

all: $(PROCED)

boot.o: boot.s
brk.o: brk.s
comm.o: comm.s
disk.o: disk.s
init.o: init.s
mch.o: mch.s
p.gesa.o: p.gesa.s
p.ldsd.o: p.ldsd.s
p.term.o: p.term.s
ram.o: ram.s
sa.test.o: sa.test.s
twem.o: twem.s
wdc.o: wdc.s
menu.o:	menu.c
misc.o:	misc.c

$(PROCED): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

clean:
	rm -f $(PROCED) boot.o  brk.o  comm.o  disk.o  init.o  mch.o  menu.o  misc.o  p.gesa.o  p.ldsd.o  p.term.o  ram.o  sa.test.o  twem.o  wdc.o

.PHONY: all clean
