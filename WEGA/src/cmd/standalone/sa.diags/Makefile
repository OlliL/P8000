# sa.diags: diagnostic program. Its start-up and driver modules (p.gesa.s,
# p.ldsd.s, p.term.s) come from the MON16 firmware and are not in this tree,
# so this build does not work yet (see src/TODO.md).

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
