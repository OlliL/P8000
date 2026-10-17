# libc: check that the sources in WEGA/src/lib/libc rebuild to the original
# members, and (re)generate the cas listings under listing/<dir>/.
#
#   make check     compare every member with the original libc member
#   make listing   cc -S (.s) and cas -l (.l) listings, checked in
WEGA ?= ../../../../wega/arc/cpio
ORIG ?= ../../../../wega/work/libs/libc
SRC  ?= ../../src/lib/libc

check:
	sh check.sh $(abspath $(WEGA)) $(abspath $(SRC)) $(abspath $(ORIG))

listing:
	LISTDIR=$(abspath listing) sh check.sh $(abspath $(WEGA)) $(abspath $(SRC)) $(abspath $(ORIG))

all:
clean:

.PHONY: all clean check listing
