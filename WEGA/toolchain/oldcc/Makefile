# oldcc: assembler sources (cas syntax) of objects that were produced by an
# OLDER cc revision than the shipped /bin/cc. Such objects cannot be rebuilt
# from C with the tools we have, so they are kept as .s files that cas turns
# into the byte-identical original object. Sources that the shipped cc
# reproduces exactly are NOT listed here.
#
#   make verify   assemble every .s with the original cas and compare with
#                 the original object ($(LIBS)/libc/<member>.o)
#   make gen      regenerate the .s from the original objects (tools/sdis.py)
WEGA ?= ../../../../wega/arc/cpio
LIBS ?= ../../../../wega/work/libs
EMU  ?= $(abspath ../z8k-emu/z8k-emu)
RUN   = $(EMU) -r $(abspath $(WEGA))
MEMBERS = $(basename $(notdir $(wildcard libc/*.s)))

verify:
	@set -e; t=$$(mktemp -d); fail=0; \
	for m in $(MEMBERS); do \
		cp libc/$$m.s $$t/; \
		(cd $$t && $(RUN) /bin/cas -u -o $$m.o $$m.s >log 2>&1); \
		if cmp -s $$t/$$m.o $(LIBS)/libc/$$m.o; then echo "ok   $$m"; \
		else echo "DIFF $$m"; fail=1; fi; \
	done; rm -rf $$t; exit $$fail

gen:
	@for m in $(MEMBERS); do python3 -I ../tools/sdis.py -a $(LIBS)/libc/$$m.o > libc/$$m.s; done

.PHONY: verify gen
