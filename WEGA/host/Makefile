
# WEGA tool chain, Linux side. Everything in here runs on the Linux host:
#
#   z8k-emu/   emulator that runs the original WEGA binaries (the oracle)
#   tools/     analysis scripts (disassembler, object and archive readers)
#   cc/        the WEGA cc driver built for Linux (cross tool)
#   popt/      the WEGA peephole optimizer built for Linux (cross tool)
#   libc/      check of the libc members in ../src/lib/libc
#   oldcc/     check of the cas sources in ../src/lib/libc/oldcc
#
# The WEGA programs themselves are built from ../src (make -C ../src/cmd);
# here they are built with the original WEGA tools (under z8k-emu) and
# compared with the originals:
#
#   make verify          all checks below
#   make verify-wega     build ../src/cmd with the original cc, cas, as, ld,
#                        ar and strip, run strip-programs, then report per
#                        program whether it is identical to the original
#                        (report only, does not fail on differences)
#
# WEGA: the WEGA file tree (bin/, lib/, usr/...), not in git.

WEGA ?= ../../../wega/arc/cpio
WEGAABS = $(abspath $(WEGA))
EMU  ?= $(abspath z8k-emu/z8k-emu)
ROOT  = $(WEGAABS)
WEMU  = $(EMU) -r $(ROOT)
SRC   = ../src/cmd

# the original tools run under the emulator
WTOOLS = CC='$(WEMU) /bin/cc' CAS='$(WEMU) /bin/cas' AS='$(WEMU) /bin/as' \
         LD='$(WEMU) /bin/ld' AR='$(WEMU) /bin/ar' STRIP='$(WEMU) /usr/bin/strip'

# program (in ../src/cmd) : original (in the WEGA tree)
WEGA_PROGS = ar:bin/ar ranlib:usr/bin/ranlib size:bin/size strip:usr/bin/strip \
	getud:bin/getud putud:bin/putud copydisk:bin/copydisk dump:etc/dump \
	dumpdir:etc/dumpdir format:bin/format restor:etc/restor sync:bin/sync \
	tar:bin/tar verify:bin/verify \
	BOOT0/pb.image:pb.image \
	doscopy/doscat:usr/bin/doscat doscopy/dosrm:usr/bin/dosrm \
	doscopy/doscp:usr/bin/doscp doscopy/dosdir:usr/bin/dosdir \
	doscopy/dosls:usr/bin/dosls doscopy/dosmkdir:usr/bin/dosmkdir \
	doscopy/dosrmdir:usr/bin/dosrmdir doscopy/dosformat:usr/bin/dosformat \
	pburn/pburn:bin/pburn \
	standalone/sa.cat:sa.cat standalone/sa.format:sa.format \
	standalone/sa.verify:sa.verify standalone/sa.mkfs:sa.mkfs \
	standalone/sa.install:sa.install standalone/sa.timer:sa.timer \
	standalone/sa.shipdisk:sa.shipdisk

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
	$(MAKE) -C $(SRC) clean >/dev/null
	-$(MAKE) -C $(SRC) all $(WTOOLS)
	-$(MAKE) -C $(SRC) strip-programs $(WTOOLS)
	@for pair in $(WEGA_PROGS); do \
	  p=$${pair%%:*}; o=$(WEGAABS)/$${pair#*:}; f=$(SRC)/$$p; \
	  if [ ! -f $$f ]; then echo "$$p: not built"; \
	  elif cmp -s $$f $$o; then echo "$$p: identical"; \
	  else echo "$$p: differs ($$(stat -c%s $$f) vs $$(stat -c%s $$o) bytes)"; fi; \
	done
	-python3 tools/cmpins.py $(SRC)/size $(WEGAABS)/bin/size 0x224 0x224
	-python3 tools/cmpins.py $(SRC)/strip $(WEGAABS)/usr/bin/strip 0x68c 0x68c

clean:
	$(MAKE) -C cc clean
	$(MAKE) -C popt clean

.PHONY: all check verify verify-wega clean cc popt
