# tools/Makefile: analysis of codgen against /lib/codgen (not part of the codgen build; see README.md).
#
#   make pipeline WEGA=dir                  build codgen, then locate, confirm, patterns, classify, inventory
#   make check WEGA=dir CORPUS=dir          codgen against the original on a test corpus (check.sh)
#   make locate|confirm|patterns|classify|inventory WEGA=dir    single steps (codgen is built if needed)
#
# WEGA: the WEGA file tree (lib/codgen is the original). CC is passed on the command line as for codgen.

CG = ..
ORIG = $(WEGA)/lib/codgen
PY = python3

pipeline: locate confirm patterns classify inventory

$(CG)/codgen:
	cd $(CG) ; $(MAKE) codgen

locate: $(CG)/codgen
	$(PY) locate.py --mine $(CG)/codgen --check

confirm: $(CG)/codgen
	$(PY) confirm.py --mine $(CG)/codgen --orig $(ORIG)

patterns: $(CG)/codgen
	$(PY) patterns.py --mine $(CG)/codgen --orig $(ORIG)

classify: $(CG)/codgen
	$(PY) classify.py --mine $(CG)/codgen --orig $(ORIG)

inventory:
	$(PY) inventory.py

check: $(CG)/codgen
	WEGA=$(WEGA) sh check.sh $(CORPUS)

.PHONY: pipeline locate confirm patterns classify inventory check
