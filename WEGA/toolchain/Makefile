# WEGA tool chain rebuild. One subdirectory per original binary.
# `make all` builds everything; `make check` runs the tool self-checks
# (needs the original WEGA files, see KNOWLEDGE.md).

SUBDIRS =

# Directory with the WEGA file tree (lib/, bin/, usr/...), not in git.
WEGA ?= ../../../wega/arc/cpio
LIBS ?= ../../../wega/work/libs

all:
	@for d in $(SUBDIRS); do $(MAKE) -C $$d all || exit 1; done

clean:
	@for d in $(SUBDIRS); do $(MAKE) -C $$d clean || exit 1; done

check:
	python3 tools/check_reloc.py $(LIBS)/*/*.o

.PHONY: all clean check
