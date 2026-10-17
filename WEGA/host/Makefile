# WEGA tool chain, Linux side. Everything in here runs on the Linux host:
#
#   z8k-emu/   emulator that runs the original WEGA binaries (the oracle)
#   tools/     analysis scripts (disassembler, object and archive readers)
#   cc/        the WEGA cc driver built for Linux (cross tool)
#   popt/      the WEGA peephole optimizer built for Linux (cross tool)
#   libc/      check of the libc members in ../src/lib/libc
#   oldcc/     check of the cas sources in ../src/lib/libc/oldcc
#
# The WEGA programs themselves are built from ../src (make -C ../src); here
# they are only verified against the original files:
#
#   make verify          all checks below
#   make verify-wega     ar, size, strip, ranlib built with the original tools
#                        (run under z8k-emu) compared with the originals
#
# WEGA: the WEGA file tree (bin/, lib/, usr/...), not in git.

WEGA ?= ../../../wega/arc/cpio
WEGAABS = $(abspath $(WEGA))
EMU  ?= $(abspath z8k-emu/z8k-emu)
ROOT  = $(WEGAABS)
WEMU  = $(EMU) -r $(ROOT)
SRC   = ../src/cmd

# the original cc, cas and ld run under the emulator
WTOOLS = CC='$(WEMU) /bin/cc' CAS='$(WEMU) /bin/cas' LD='$(WEMU) /bin/ld'

all: cc popt

cc popt:
	$(MAKE) -C $@ all

check:
	$(MAKE) -C cc check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C popt check WEGA=$(WEGAABS)
	$(MAKE) -C libc check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C oldcc verify WEGA=$(WEGAABS) EMU=$(EMU)

verify: all check verify-wega

verify-wega:
	$(MAKE) -B -C $(SRC) ar ranlib size strip $(WTOOLS)
	cmp $(SRC)/ar $(WEGAABS)/bin/ar && echo "ar: identical to original"
	python3 tools/cmpins.py $(SRC)/size $(WEGAABS)/bin/size 0x224 0x224
	python3 tools/cmpins.py $(SRC)/strip $(WEGAABS)/usr/bin/strip 0x68c 0x68c
	$(MAKE) -C $(SRC) clean >/dev/null

clean:
	$(MAKE) -C cc clean
	$(MAKE) -C popt clean

.PHONY: all check verify verify-wega clean cc popt
