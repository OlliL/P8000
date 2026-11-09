
# WEGA tool chain, Linux side. Everything in here runs on the Linux host:
#
#   z8k-emu/   emulator that runs the original WEGA binaries (the oracle)
#   tools/     analysis scripts (disassembler, object and archive readers)
#   cc/        the WEGA cc and scc drivers built for Linux (cross tools)
#   ar/        the WEGA ar built for Linux (cross tool)
#   binutils/  the WEGA ranlib, strip and size built for Linux (cross tools)
#   popt/      the WEGA peephole optimizer built for Linux (cross tool)
#   codgen/    the WEGA code generators codgen and scodgen built for Linux (cross tools)
#   cparse/    the WEGA front ends cparse and scparse built for Linux (cross tools)
#   cas/       the WEGA assembler cas built for Linux (cross tool)
#   ld/        the WEGA loaders ld, sld and gld built for Linux (cross tools)
#
# The WEGA programs themselves are built from ../src (make -C ../src/cmd);
# here they are built with the original WEGA tools (under z8k-emu) and
# compared with the originals:
#
#   make checkall        everything that is byte-identical, each program with its compiler, on a copy of the
#                        files in git, and the corpora (tools/checkall.sh): the check to run before a commit
#   make oldroot         the old compilers and libraries from the sources, into OLDROOT: a WEGA tree of symbolic links to WEGA
#                        (tools/oldroot.sh) with bin/ccvN and lib/vN/{cparse,codgen,popt}, and src/lib: stage1 lib stage2 of ../src/Makefile;
#                        then: z8k-emu -r OLDROOT /bin/ccv1 -c -O foo.c
#   make src             the same and ../src/cmd
#

# WEGA: the original WEGA 3.1 file tree (ROOT of the emulator); default: the placeholder of CLAUDE.md (tmp/wega31_root, not in git)
WEGA = ../../tmp/wega31_root
WEGAABS = $(abspath $(WEGA))
EMU  = $(abspath z8k-emu/z8k-emu)

# the WEGA tree of the old compiler versions (not in git)
OLDROOT = ../../tmp/oldroot
OLDROOTABS = $(abspath $(OLDROOT))

# the original tools under the emulator, for the PATH of make (tools/wbin.sh); the Makefiles of src use the plain names
WBIN = $(abspath ../../tmp/wbin)
WPATH = PATH=$(WBIN):$$PATH

all: cc popt codgen cparse ar binutils cas ld cpp

cc popt codgen cparse ar binutils cas ld cpp:
	$(MAKE) -C $@ all

checkall:
	WEGA=$(WEGAABS) sh tools/checkall.sh

wbin:
	sh tools/oldroot.sh $(WEGAABS) $(OLDROOTABS)
	sh tools/wbin.sh $(WBIN) $(WEGAABS) $(OLDROOTABS) $(EMU)

oldroot: wbin
	$(WPATH) $(MAKE) -C ../src MAKEARGS="-j8" stage1 lib stage2 OLDROOT=$(OLDROOTABS)

src: wbin
	$(WPATH) $(MAKE) -C ../src MAKEARGS="-j8" all OLDROOT=$(OLDROOTABS)

clean:
	$(MAKE) -C cc clean
	$(MAKE) -C ar clean
	$(MAKE) -C binutils clean
	$(MAKE) -C popt clean
	$(MAKE) -C codgen clean
	$(MAKE) -C cparse clean
	$(MAKE) -C cas clean
	$(MAKE) -C cpp clean

.PHONY: all checkall wbin oldroot src clean cc popt codgen cparse ar binutils cas ld cpp
