# cpp: the C preprocessor of WEGA (/lib/cpp) built for Linux (a cross tool: it behaves like the original).
# Sources: cpp.c, cpy.y and yylex.c, taken over from ../../src/cmd/cpp by the port rules in
# ../HANDOFF_wega_disasm.md. cpy.y is in the dialect of the host yacc (bison -y).
#
#   make                 build ./cpp
#   make check           compare with the original /lib/cpp under z8k-emu on the corpus of
#                        ../../src/cmd/cpp/tools (WEGA=<file tree of the original system>)
#   make clean

HOSTCC = cc
YACC = bison -y
CFLAGS = -O2 -Wall -Wextra -std=gnu99 -fsigned-char
WEGA = ../../../tmp/wega31_root

all: cpp

cpy.c: cpy.y
	$(YACC) -o $@ cpy.y

cpp: cpp.c cpy.c yylex.c
	$(HOSTCC) $(CFLAGS) -I. -o $@ cpp.c cpy.c

check: cpp
	WEGA_ROOT=$(abspath $(WEGA)) sh ../tools/corpus.sh --host cpp cpp

clean:
	rm -f cpp cpy.c

.PHONY: all check clean
