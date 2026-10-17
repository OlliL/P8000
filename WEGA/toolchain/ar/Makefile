# ar: archive maintainer (System III/V7 ar 2.2 as shipped with WEGA 3.2).
#
#   make all      rebuild with the ORIGINAL WEGA cc/cas/ld run under z8k-emu
#                 (needs the WEGA tree, see ../KNOWLEDGE.md); result build/ar
#   make verify   compare build/ar with the original $(WEGA)/bin/ar
#
# crt0v.s is the start-up code variant found in this binary (not the
# standard crt0). The modern Linux port is added later.

WEGA ?= ../../../../wega/arc/cpio
EMU  ?= $(abspath ../z8k-emu/z8k-emu)
RUN   = $(EMU) -r $(abspath $(WEGA))
WORK ?= build

all: $(WORK)/ar

$(WORK)/ar: $(WORK)/ar.o $(WORK)/crt0v.o
	cd $(WORK) && $(RUN) /bin/ld -s -e start -o ar crt0v.o ar.o /lib/libc.a

$(WORK)/ar.o: ar.c
	mkdir -p $(WORK)
	cp ar.c $(WORK)/ar.c
	cd $(WORK) && $(RUN) /bin/cc -O -c ar.c

$(WORK)/crt0v.o: crt0v.s
	mkdir -p $(WORK)
	cp crt0v.s $(WORK)/crt0v.s
	cd $(WORK) && $(RUN) /bin/cas -u -o crt0v.o crt0v.s

verify: all
	cmp $(WORK)/ar $(WEGA)/bin/ar && echo "ar: identical to original"

# cas listings (cc -S source and cas -l listing) of the rebuilt sources,
# checked in under listing/ to ease comparison with the originals
listing:
	../tools/listing.sh $(abspath $(WEGA)) ar.c listing -O
	../tools/listing.sh $(abspath $(WEGA)) crt0v.s listing

clean:
	rm -rf $(WORK)

.PHONY: all verify clean listing
