# libc: check that the sources in WEGA/src/lib/libc rebuild to the original
# members.
#
#   make check     compare every member with the member of the original /lib/libc.a (extracted into orig/)
WEGA ?= ../../../tmp/wega31_root
EMU  ?= ../z8k-emu/z8k-emu
SRC  ?= ../../src/lib/libc

check:
	mkdir -p orig
	cd orig && $(abspath $(EMU)) -r $(abspath $(WEGA)) /bin/ar x /lib/libc.a
	sh check.sh $(abspath $(WEGA)) $(abspath $(SRC)) $(abspath orig)

all:
clean:

.PHONY: all clean check
