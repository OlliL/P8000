# cc: the WEGA C compiler driver built for Linux (a cross tool: it behaves
# like /bin/cc of WEGA, it is only compiled by the host cc). Source:
# ../../src/cmd/cc/cc.c. The WEGA header set lacks some headers the driver
# uses; they are supplied here, so the source stays WEGA-compatible.
#
#   make                 build ./cc
#   make check           compare with the original /bin/cc under z8k-emu

HOSTCC ?= cc
CFLAGS ?= -O2 -w
SRC = ../../src/cmd/cc
WEGA ?= ../../../../wega/arc/cpio
EMU  ?= $(abspath ../z8k-emu/z8k-emu)

HOSTINC = -include stdio.h -include ctype.h -include signal.h -include errno.h \
	-include string.h -include stdlib.h -include unistd.h \
	-include sys/types.h -include sys/wait.h

all: cc

cc: $(SRC)/cc.c
	$(HOSTCC) $(CFLAGS) -std=gnu89 $(HOSTINC) -o $@ $(SRC)/cc.c

check: cc
	WEGA=$(abspath $(WEGA)) E=$(EMU) sh check.sh

clean:
	rm -f cc

.PHONY: all check clean
