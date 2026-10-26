# oldcc: assembler sources (cas syntax) of the members of the shipped libc.a that were produced by an
# OLDER cc revision than the shipped /bin/cc (directory src/lib/libc/oldcc/<member>/<version>.s). Such objects cannot be rebuilt
# from C with the tools we have, so they are kept as .s files that cas turns
# into the byte-identical original object. Sources that the shipped cc
# reproduces exactly are NOT listed here.
#
#   make libs     extract the original members of /lib/libc.a (WEGA 3.1 tree, under the emulator) into $(LIBS)/libc
#   make verify   assemble every .s with the original cas and compare with the original object $(LIBS)/libc/<member>.o
#   make gen      regenerate the .s from the original objects (tools/sdis.py)
#
# WEGA is the original WEGA 3.1 file tree (ROOT of the emulator); the default is the placeholder of CLAUDE.md
# (tmp/wega31_root, not in git). LIBS is a scratch directory below tmp/ (not in git).
WEGA = ../../../tmp/wega31_root
LIBS = ../../../tmp/oldcc-libs
EMU  = $(abspath ../z8k-emu/z8k-emu)
RUN  = $(EMU) -r $(abspath $(WEGA))
SRC  = ../../src/lib/libc/oldcc
MEMBERS = cnvt doscan fltioc length malloc nlist printf scanf swap symscan

libs:
	mkdir -p $(LIBS)/libc
	cd $(LIBS)/libc && $(RUN) /bin/ar x /lib/libc.a
	@for m in $(MEMBERS); do test -f $(LIBS)/libc/$$m.o || { echo "missing $$m.o"; exit 1; }; done

verify: libs
	@set -e; t=$$(mktemp -d); fail=0; \
	for m in $(MEMBERS); do for f in $(SRC)/$$m/*.s; do \
		cp $$f $$t/$$m.s; \
		(cd $$t && $(RUN) /bin/cas -u -o $$m.o $$m.s >log 2>&1); \
		if cmp -s $$t/$$m.o $(LIBS)/libc/$$m.o; then echo "ok   $$m ($$(basename $$f))"; \
		else echo "DIFF $$m ($$(basename $$f))"; fail=1; fi; \
	done; done; rm -rf $$t; exit $$fail

gen: libs
	@for m in $(MEMBERS); do for f in $(SRC)/$$m/*.s; do python3 -I ../tools/sdis.py -a $(LIBS)/libc/$$m.o > $$f; done; done

.PHONY: libs verify gen
