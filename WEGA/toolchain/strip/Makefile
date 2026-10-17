# strip: remove symbols/relocation (and with -h the header) from s.out files.
#
#   make all      rebuild with the ORIGINAL WEGA cc/cas/ld run under z8k-emu
#   make verify   compare the own code (0x42..fseek) with the original
#                 $(WEGA)/usr/bin/strip; the libc part of the original is an older
#                 libc revision, so the whole file is not identical.

WEGA ?= ../../../../wega/arc/cpio
EMU  ?= $(abspath ../z8k-emu/z8k-emu)
RUN   = $(EMU) -r $(abspath $(WEGA))
WORK ?= build

all: $(WORK)/strip

$(WORK)/strip: $(WORK)/strip.o $(WORK)/crt0s.o
	cd $(WORK) && $(RUN) /bin/ld -s -e start -o strip crt0s.o strip.o /lib/libc.a

$(WORK)/strip.o: strip.c
	mkdir -p $(WORK)
	cp strip.c $(WORK)/strip.c
	cd $(WORK) && $(RUN) /bin/cc -O -c strip.c

$(WORK)/crt0s.o: crt0s.s
	mkdir -p $(WORK)
	cp crt0s.s $(WORK)/crt0s.s
	cd $(WORK) && $(RUN) /bin/cas -u -o crt0s.o crt0s.s

verify: all
	python3 ../tools/cmpins.py $(WORK)/strip $(WEGA)/usr/bin/strip 0x68c 0x68c

# cas listings (cc -S source and cas -l listing) of the rebuilt sources,
# checked in under listing/ to ease comparison with the originals
listing:
	../tools/listing.sh $(abspath $(WEGA)) strip.c listing -O
	../tools/listing.sh $(abspath $(WEGA)) crt0s.s listing

clean:
	rm -rf $(WORK)

.PHONY: all verify clean listing
