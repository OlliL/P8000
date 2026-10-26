# popt analysis, the same setup as codgen/tools: the symbol build, the location of our functions, the candidates
# of the original, the classification.
# The original /lib/popt is stripped (no symbol table): the starts of its functions are found by analysis
# (pair.py candidates, then confirmation by hand in findings.json).
# Run in this directory, WEGA = the WEGA 3.1 tree, EMU = the z8k emulator with the WEGA root:
#   make pipeline WEGA=<WEGA_ROOT> EMU="<z8k-emu> -r <WEGA_ROOT>"
# Native make: '=' assignments, no GNU extensions.

WEGA = /mnt/share/cvs/P8000/tmp/wega31_root
EMU  = /mnt/share/cvs/P8000/WEGA/host/z8k-emu/z8k-emu -r $(WEGA)
WORK = /mnt/share/cvs/P8000/tmp/tmp/popt
SYM  = $(WORK)/symbuild

symbuild:
	python3 mksym.py .. $(SYM)
	cd $(SYM) ; $(EMU) /bin/cc -c -O z20.c ; $(EMU) /bin/cc -c -O z21.c ; $(EMU) /bin/cc -c -O z22.c ; $(EMU) /bin/cc -c -O z23.c
	cd $(SYM) ; $(EMU) /bin/ld -X -e start -o popt_sym /lib/crt0.o z20.o z21.o z22.o z23.o /lib/libc.a

locate: symbuild
	python3 locate.py --mine $(SYM)/popt_sym --update --check

pair: locate
	python3 pair.py --mine $(SYM)/popt_sym --orig $(WEGA)/lib/popt --write

classify: pair
	python3 classify.py --mine $(SYM)/popt_sym --orig $(WEGA)/lib/popt
	python3 patterns.py --mine $(SYM)/popt_sym --orig $(WEGA)/lib/popt

pipeline: classify
	python3 findings_md.py

.PHONY: symbuild locate pair classify pipeline
