# WEGA tool chain rebuild. One subdirectory per original binary.
# `make all` builds everything; `make check` runs the tool self-checks
# (needs the original WEGA files, see KNOWLEDGE.md).

SUBDIRS = z8k-emu ar size strip

# Directory with the WEGA file tree (lib/, bin/, usr/...), not in git.
WEGA ?= ../../../wega/arc/cpio
LIBS ?= ../../../wega/work/libs

all:
	@for d in $(SUBDIRS); do $(MAKE) -C $$d all || exit 1; done

clean:
	@for d in $(SUBDIRS); do $(MAKE) -C $$d clean || exit 1; done

check:
	python3 tools/check_reloc.py $(LIBS)/*/*.o

# Oracle self test (needs UTS= extracted uts.tar.gz/uts and KOBJ= kernel lib2 dir)
verify: all
	@for d in $(SUBDIRS); do [ ! -f $$d/Makefile ] || $(MAKE) -C $$d verify WEGA=$(abspath $(WEGA)) 2>/dev/null || true; done

check-oracle: all
	z8k-emu/selftest.sh $(WEGA) $(UTS) $(KOBJ)

.PHONY: all clean check check-oracle
