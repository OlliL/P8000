# libc: check that the sources in WEGA/src/lib/libc rebuild to the original
# members.
#
#   make check     compare every member with the original libc member
WEGA ?= ../../../../wega/arc/cpio
ORIG ?= ../../../../wega/work/libs/libc
SRC  ?= ../../src/lib/libc

check:
	sh check.sh $(abspath $(WEGA)) $(abspath $(SRC)) $(abspath $(ORIG))

all:
clean:

.PHONY: all clean check
