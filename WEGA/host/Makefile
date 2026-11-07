
# WEGA tool chain, Linux side. Everything in here runs on the Linux host:
#
#   z8k-emu/   emulator that runs the original WEGA binaries (the oracle)
#   tools/     analysis scripts (disassembler, object and archive readers)
#   cc/        the WEGA cc and scc drivers built for Linux (cross tools)
#   ar/        the WEGA ar built for Linux (cross tool)
#   binutils/  the WEGA ranlib, strip and size built for Linux (cross tools)
#   popt/      the WEGA peephole optimizer built for Linux (cross tool)
#   cas/       the WEGA assembler cas built for Linux (cross tool)
#   ld/        the WEGA loaders ld, sld and gld built for Linux (cross tools)
#   libc/      check of the libc members in ../src/lib/libc
#   oldcc/     check of the cas sources in ../src/lib/libc/oldcc
#
# The WEGA programs themselves are built from ../src (make -C ../src/cmd);
# here they are built with the original WEGA tools (under z8k-emu) and
# compared with the originals:
#
#   make checkall        everything that is byte-identical, each program with its compiler, on a copy of the
#                        files in git, and the corpora (tools/checkall.sh): the check to run before a commit
#   make verify          all checks below
#   make verify-wega     build ../src/cmd, then report per program in WEGA_PROGS whether it is identical to the original
#                        (report only; the identical ones are in checkall); needs make oldroot
#   make oldroot         the old compilers and libraries from the sources, into OLDROOT: a WEGA tree of symbolic links to WEGA
#                        (tools/oldroot.sh) with bin/ccvN and lib/vN/{cparse,codgen,popt}, and src/lib: stage1 lib stage2 of ../src/Makefile;
#                        then: z8k-emu -r OLDROOT /bin/ccv1 -c -O foo.c
#   make src             the same and ../src/cmd
#

# WEGA: the original WEGA 3.1 file tree (ROOT of the emulator); default: the placeholder of CLAUDE.md (tmp/wega31_root, not in git)
WEGA = ../../tmp/wega31_root
WEGAABS = $(abspath $(WEGA))
EMU  = $(abspath z8k-emu/z8k-emu)
ROOT  = $(WEGAABS)
WEMU  = $(EMU) -r $(ROOT)
SRC   = ../src/cmd

# the WEGA tree of the old compiler versions (not in git)
OLDROOT = ../../tmp/oldroot
OLDROOTABS = $(abspath $(OLDROOT))

# the original tools under the emulator, for the PATH of make (tools/wbin.sh); the Makefiles of src use the plain names
WBIN = $(abspath ../../tmp/wbin)
WPATH = PATH=$(WBIN):$$PATH

# program (in ../src/cmd) : original (in the WEGA tree); commands with several
# versions are built in the subdirectory of the version shipped with WEGA 3.1
WEGA_PROGS = dc/dc:bin/dc ar:bin/ar cc/build/nonseg/cc:bin/cc cc/build/seg/scc:bin/scc ranlib/ranlib:usr/bin/ranlib \
	getud:bin/getud putud:bin/putud copydisk/1.5/copydisk:bin/copydisk dump:etc/dump \
	dumpdir:etc/dumpdir format/1.5/format:bin/format restor:etc/restor sync:bin/sync \
	tar:bin/tar verify/1.5/verify:bin/verify cas/build/cas:bin/cas \
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

checkall:
	WEGA=$(WEGAABS) sh tools/checkall.sh

wbin:
	sh tools/oldroot.sh $(WEGAABS) $(OLDROOTABS)
	sh tools/wbin.sh $(WBIN) $(WEGAABS) $(OLDROOTABS) $(EMU)

verify-wega: wbin
	$(MAKE) -C $(SRC) clean >/dev/null
	-$(WPATH) $(MAKE) -C $(SRC) all
	@for pair in $(WEGA_PROGS); do \
	  p=$${pair%%:*}; o=$(WEGAABS)/$${pair#*:}; f=$(SRC)/$$p; \
	  if [ ! -f $$f ]; then echo "$$p: not built"; \
	  elif cmp -s $$f $$o; then echo "$$p: identical"; \
	  else echo "$$p: differs ($$(stat -c%s $$f) vs $$(stat -c%s $$o) bytes)"; fi; \
	done
	@echo 'ranlib (not identical yet): the libc part linked from the old libc modules:'
	-python3 tools/cmpins.py $(SRC)/ranlib/ranlib $(WEGAABS)/usr/bin/ranlib 0x1bce 0x1bd4 0x534 0x53a

oldroot: wbin
	$(WPATH) $(MAKE) -C ../src stage1 lib stage2 OLDROOT=$(OLDROOTABS)

src: wbin
	$(WPATH) $(MAKE) -C ../src all OLDROOT=$(OLDROOTABS)

clean:
	$(MAKE) -C cc clean
	$(MAKE) -C ar clean
	$(MAKE) -C binutils clean
	$(MAKE) -C popt clean
	$(MAKE) -C cas clean

.PHONY: all check checkall verify verify-wega wbin oldroot src clean cc popt ar binutils cas
