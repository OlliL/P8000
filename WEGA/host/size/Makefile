# size: print the segment sizes of s.out files.
#
#   make all      rebuild with the ORIGINAL WEGA cc/cas/ld run under z8k-emu
#   make verify   compare the own code (0x42..0x224) with the original
#                 $(WEGA)/bin/size (the libc part of the original is an older
#                 libc revision, so the whole file is not identical).

WEGA ?= ../../../../wega/arc/cpio
EMU  ?= $(abspath ../z8k-emu/z8k-emu)
RUN   = $(EMU) -r $(abspath $(WEGA))
WORK ?= build
# the WEGA source of the program
SRC  ?= ../../src/cmd/size

all: $(WORK)/size

$(WORK)/size: $(WORK)/size.o $(WORK)/crt0s.o
	cd $(WORK) && $(RUN) /bin/ld -s -e start -o size crt0s.o size.o /lib/libc.a

$(WORK)/size.o: $(SRC)/size.c
	mkdir -p $(WORK)
	cp $(SRC)/size.c $(WORK)/size.c
	cd $(WORK) && $(RUN) /bin/cc -O -c size.c

$(WORK)/crt0s.o: $(SRC)/crt0s.s
	mkdir -p $(WORK)
	cp $(SRC)/crt0s.s $(WORK)/crt0s.s
	cd $(WORK) && $(RUN) /bin/cas -u -o crt0s.o crt0s.s

verify: all
	python3 ../tools/cmpins.py $(WORK)/size $(WEGA)/bin/size 0x224 0x224

.PHONY: all verify clean
