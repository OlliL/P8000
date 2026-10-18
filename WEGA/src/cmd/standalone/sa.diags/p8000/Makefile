# sa.diags: diagnostic program shipped with WEGA 3.1

AS = as   # the original standalone build assembles with as
CC = cc
CFLAGS =
LD = ld
LDFLAGS = -b 0x8000 -e _main
PREFIX = /

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

boot.o:
	$(AS) -u -o boot.o boot.s

brk.o:
	$(AS) -u -o brk.o brk.s

comm.o:
	$(AS) -u -o comm.o comm.s

disk.o:
	$(AS) -u -o disk.o disk.s

init.o:
	$(AS) -u -o init.o init.s

mch.o:
	$(AS) -u -o mch.o mch.s

ram.o:
	$(AS) -u -o ram.o ram.s

sa.diags.o:
	$(AS) -u -o sa.diags.o sa.diags.s

sa.test.o:
	$(AS) -u -o sa.test.o sa.test.s

wdc.o:
	$(AS) -u -o wdc.o wdc.s

menu.o:	menu.c
	$(CC) $(CFLAGS) -c menu.c

misc.o:	misc.c
	$(CC) $(CFLAGS) -c misc.c

$(PROCED): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

clean:
	rm -f $(PROCED) boot.o  brk.o  comm.o  disk.o  gesa.o  init.o  ldsd.o  mch.o  menu.o  misc.o  ram.o  sa.test.o  term.o  wdc.o term.s ldsd.s gesa.s

.PHONY: all clean
