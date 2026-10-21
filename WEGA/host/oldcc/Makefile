# oldcc: assembler sources (cas syntax) of the members of the shipped libc.a that were produced by an
# OLDER cc revision than the shipped /bin/cc (directory src/lib/libc/oldcc/<member>/<version>.s). Such objects cannot be rebuilt
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
SRC  ?= ../../src/lib/libc/oldcc
MEMBERS = $(notdir $(wildcard $(SRC)/*))

verify:
	@set -e; t=$$(mktemp -d); fail=0; \
	for m in $(MEMBERS); do for f in $(SRC)/$$m/*.s; do \
		cp $$f $$t/$$m.s; \
		(cd $$t && $(RUN) /bin/cas -u -o $$m.o $$m.s >log 2>&1); \
		if cmp -s $$t/$$m.o $(LIBS)/libc/$$m.o; then echo "ok   $$m ($$(basename $$f))"; \
		else echo "DIFF $$m ($$(basename $$f))"; fail=1; fi; \
	done; done; rm -rf $$t; exit $$fail

gen:
	@for m in $(MEMBERS); do for f in $(SRC)/$$m/*.s; do python3 -I ../tools/sdis.py -a $(LIBS)/libc/$$m.o > $$f; done; done

.PHONY: verify gen
