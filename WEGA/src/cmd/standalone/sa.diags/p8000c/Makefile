# sa.diags: diagnostic program extended with WDOS Test, meant for
# the P8000 compact (but got never released)

AS ?= as   # the original standalone build assembles with as
CC ?= cc
CFLAGS ?= -O
LD ?= ld
LDFLAGS = -s -b 0x8000 -e _main
PREFIX ?= /

OBJECTS = menu.o sa.test.o twem.o wdc.o init.o p.term.o comm.o brk.o \
          p.gesa.o boot.o disk.o p.ldsd.o misc.o mch.o ram.o
PROCED = sa.diags

all: $(PROCED)

.s.o:
	$(AS) -u -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROCED): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

clean:
	rm -f $(PROCED) *.o

.PHONY: all clean
