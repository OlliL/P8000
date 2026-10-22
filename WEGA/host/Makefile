
# WEGA tool chain, Linux side. Everything in here runs on the Linux host:
#
#   z8k-emu/   emulator that runs the original WEGA binaries (the oracle)
#   tools/     analysis scripts (disassembler, object and archive readers)
#   cc/        the WEGA cc and scc drivers built for Linux (cross tools)
#   ar/        the WEGA ar built for Linux (cross tool)
#   binutils/  the WEGA ranlib, strip and size built for Linux (cross tools)
#   popt/      the WEGA peephole optimizer built for Linux (cross tool)
#   cas/       the WEGA assembler cas built for Linux (cross tool)
#   ld/        the WEGA non-segmented loader ld built for Linux (cross tool)
#   libc/      check of the libc members in ../src/lib/libc
#   oldcc/     check of the cas sources in ../src/lib/libc/oldcc
#
# The WEGA programs themselves are built from ../src (make -C ../src/cmd);
# here they are built with the original WEGA tools (under z8k-emu) and
# compared with the originals:
#
#   make verify          all checks below
#   make verify-wega     build ../src/cmd with the original cc, scc, cas, as,
#                        ld, sld, gld, yacc, ar and strip, then report per program
#                        whether it is identical to the original
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
WTOOLS = CC='$(WEMU) /bin/cc' SCC='$(WEMU) /bin/scc' CAS='$(WEMU) /bin/cas' \
         AS='$(WEMU) /bin/as' LD='$(WEMU) /bin/ld' SLD='$(WEMU) /bin/sld' \
         GLD='$(WEMU) /bin/gld' YACC='$(WEMU) /usr/bin/yacc' AR='$(WEMU) /bin/ar' \
         STRIP='$(WEMU) /usr/bin/strip'

# program (in ../src/cmd) : original (in the WEGA tree); commands with several
# versions are built in the subdirectory of the version shipped with WEGA 3.1
WEGA_PROGS = ar:bin/ar cc/cc:bin/cc cc/scc:bin/scc ranlib/ranlib:usr/bin/ranlib size/size:bin/size strip/strip:usr/bin/strip \
	getud:bin/getud putud:bin/putud copydisk/1.5/copydisk:bin/copydisk dump:etc/dump \
	dumpdir:etc/dumpdir format/1.5/format:bin/format restor:etc/restor sync:bin/sync \
	tar:bin/tar verify/1.5/verify:bin/verify cas/cas:bin/cas \
	BOOT0/pb.image:pb.image \
	doscopy/doscat:usr/bin/doscat doscopy/dosrm:usr/bin/dosrm \
	doscopy/doscp:usr/bin/doscp doscopy/dosdir:usr/bin/dosdir \
	doscopy/dosls:usr/bin/dosls doscopy/dosmkdir:usr/bin/dosmkdir \
	doscopy/dosrmdir:usr/bin/dosrmdir doscopy/dosformat:usr/bin/dosformat \
	pburn/2.1/pburn:bin/pburn \
	standalone/sa.cat:sa.cat standalone/sa.format:sa.format \
	standalone/sa.verify:sa.verify standalone/sa.mkfs:sa.mkfs \
	standalone/sa.install:sa.install standalone/sa.timer:sa.timer \
	standalone/sa.shipdisk:sa.shipdisk

all: cc popt ar binutils cas ld

cc popt ar binutils cas ld:
	$(MAKE) -C $@ all

check:
	$(MAKE) -C cc check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C ar check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C binutils check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C cas check WEGA=$(WEGAABS)
	$(MAKE) -C ld check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C popt check WEGA=$(WEGAABS)
	$(MAKE) -C libc check WEGA=$(WEGAABS) EMU=$(EMU)
	$(MAKE) -C oldcc verify WEGA=$(WEGAABS) EMU=$(EMU)

verify: all check verify-wega

verify-wega:
	$(MAKE) -C $(SRC) clean >/dev/null
	-$(MAKE) -C $(SRC) all $(WTOOLS)
	@for pair in $(WEGA_PROGS); do \
	  p=$${pair%%:*}; o=$(WEGAABS)/$${pair#*:}; f=$(SRC)/$$p; \
	  if [ ! -f $$f ]; then echo "$$p: not built"; \
	  elif cmp -s $$f $$o; then echo "$$p: identical"; \
	  else echo "$$p: differs ($$(stat -c%s $$f) vs $$(stat -c%s $$o) bytes)"; fi; \
	done
	-python3 tools/cmpins.py $(SRC)/size/size $(WEGAABS)/bin/size 0x224 0x224
	-python3 tools/cmpins.py $(SRC)/strip/strip $(WEGAABS)/usr/bin/strip 0x68c 0x68c
	@echo 'size, strip, ranlib (not identical yet): the libc part linked from the old libc modules:'
	-python3 tools/cmpins.py $(SRC)/size/size $(WEGAABS)/bin/size 0x1614 0x1614 0x224
	-python3 tools/cmpins.py $(SRC)/strip/strip $(WEGAABS)/usr/bin/strip 0x1fd2 0x1fd2 0x68c
	-python3 tools/cmpins.py $(SRC)/ranlib/ranlib $(WEGAABS)/usr/bin/ranlib 0x1bce 0x1bd4 0x534 0x53a

clean:
	$(MAKE) -C cc clean
	$(MAKE) -C ar clean
	$(MAKE) -C binutils clean
	$(MAKE) -C popt clean
	$(MAKE) -C cas clean

.PHONY: all check verify verify-wega clean cc popt ar binutils cas
