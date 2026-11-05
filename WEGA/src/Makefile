# WEGA src: the libraries and the commands, each with the compiler and the libc of its time (host/OLDCC.md).
#
#   make                  everything, in this order:
#   make stage1           the old compilers including their cas, with the shipped tools and the shipped /lib/libc.a; installed below PREFIX.
#                         Only sprintf comes from the old printf: popt uses the value of sprintf, the buffer; the shipped one returns a count.
#   make lib              lib/libc/libc.a and lib/libc-vN.a, with the shipped tools and the old compilers
#   make stage2           the old compilers once more, now with the libc of their time; installed below PREFIX
#   make cmds             cmd/
#   make clean
#
# PREFIX: where the old compilers go (bin/ccvN, lib/vN/, lib/old/cas). Under Linux all tools come through host/tools/wbin.sh, see host/Makefile.

CC = cc
PREFIX = /
VERSIONS = v1 v0 v0p
OLDPRINTF = lib/oldlibc/printf/s1.o

all: stage1 lib stage2 cmds

stage1:
	cd lib/oldlibc/printf ; cp 0_5.c s1.c ; $(CC) -O -c s1.c ; rm -f s1.c
	cd cmd/cas/old ; $(MAKE) install PREFIX=$(PREFIX)
	cd cmd/cparse ; $(MAKE) oldprep
	cd cmd/codgen ; $(MAKE) oldprep
	for p in cparse codgen popt cc ; do for v in $(VERSIONS) ; do \
		( cd cmd/$$p/$$v ; $(MAKE) clean ; $(MAKE) install PREFIX=$(PREFIX) LIBC='../../../$(OLDPRINTF) /lib/libc.a' ) || exit 1 ; \
	done ; done
	for p in codgen cc ; do \
		( cd cmd/$$p/v1q ; $(MAKE) clean ; $(MAKE) install PREFIX=$(PREFIX) LIBC='../../../$(OLDPRINTF) /lib/libc.a' ) || exit 1 ; \
	done

lib:
	cd lib ; $(MAKE)

stage2:
	for p in cparse codgen popt cc ; do for v in $(VERSIONS) ; do \
		( cd cmd/$$p/$$v ; $(MAKE) clean ; $(MAKE) install PREFIX=$(PREFIX) ) || exit 1 ; \
	done ; done
	for p in codgen cc ; do \
		( cd cmd/$$p/v1q ; $(MAKE) clean ; $(MAKE) install PREFIX=$(PREFIX) ) || exit 1 ; \
	done

cmds:
	cd cmd ; $(MAKE) all

clean:
	cd lib ; $(MAKE) clean
	cd cmd ; $(MAKE) clean

.PHONY: all stage1 lib stage2 cmds clean
