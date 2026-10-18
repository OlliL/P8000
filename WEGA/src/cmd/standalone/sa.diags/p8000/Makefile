# sa.diags: diagnostic program shipped with WEGA 3.1

AS ?= as   # the original standalone build assembles with as
CC ?= cc
CFLAGS ?= 
LD ?= ld
LDFLAGS = -b 0x8000 -e _main
PREFIX ?= /

OBJECTS = menu.o sa.test.o init.o term.o comm.o brk.o \
          gesa.o boot.o disk.o ldsd.o misc.o mch.o ram.o wdc.o
PROCED = sa.diags

all: $(PROCED)

gesa.o: p.gesa.s
	cp p.gesa.s gesa.s
	$(AS) -o $@ gesa.s

ldsd.o: p.ldsd.s
	cp p.ldsd.s ldsd.s
	$(AS) -o $@ ldsd.s

term.o: p.term.s
	cp p.term.s term.s
	$(AS) -o $@ term.s

.s.o:
	$(AS) -u -o $@ $<

.c.o:
	$(CC) $(CFLAGS) -c $<

$(PROCED): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

clean:
	rm -f $(PROCED) *.o term.s ldsd.s gesa.s

.PHONY: all clean
