# WEGA src: the libraries and the commands, each with the compiler and the libc of its time.
#
#   make                  everything, in this order:
#   make stage1           the old compilers including their cas, with the shipped tools and the shipped /lib/libc.a; installed below OLDROOT.
#                         Only sprintf comes from the old printf: popt uses the value of sprintf, the buffer; the shipped one returns a count.
#   make lib              lib/libc/libc.a and lib/libc-vN.a, with the shipped tools and the old compilers
#   make stage2           the old compilers once more, now with the libc of their time; installed below OLDROOT
#   make cmds             cmd/
#   make install          the shipped programs and libc.a of cmd/ and lib/, stripped as the originals, below PREFIX
#   make clean
#
# OLDROOT: where the old compilers go (bin/ccvN, lib/vN/, lib/old/cas). PREFIX: the root of make install.

CC = cc
PREFIX = /
OLDROOT = /
OLDPRINTF = lib/oldlibc/printf/s1.o
MAKEARGS =

all: stage1 lib stage2 cmds

stage1: clean
	cd lib/oldlibc/printf ; cp 0_5.c s1.c ; $(CC) -O -c s1.c ; rm -f s1.c
	cd cmd/cas    ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT)

	cd cmd/cparse ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a'
	cd cmd/codgen ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a'
	cd cmd/popt   ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a'
	cd cmd/cc     ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a'

lib: FORCE
        # not everything in lib/ is threadsafe
	cd lib ; $(MAKE) MAKEARGS=$(MAKEARGS)

stage2:
	cd cmd/cparse ; $(MAKE) clean ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) old
	cd cmd/codgen ; $(MAKE) clean ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) old
	cd cmd/popt   ; $(MAKE) clean ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) old
	cd cmd/cc     ; $(MAKE) clean ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) old

	cd cmd/cparse ; $(MAKE) oldinstall PREFIX=$(OLDROOT)
	cd cmd/codgen ; $(MAKE) oldinstall PREFIX=$(OLDROOT)
	cd cmd/popt   ; $(MAKE) oldinstall PREFIX=$(OLDROOT)
	cd cmd/cc     ; $(MAKE) oldinstall PREFIX=$(OLDROOT)

cmds:
	cd cmd ; $(MAKE) all

install:
	cd lib ; $(MAKE) install PREFIX=$(PREFIX)
	cd cmd ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) install PREFIX=$(PREFIX)

clean:
	cd lib ; $(MAKE) clean
	cd cmd ; $(MAKE) clean

FORCE:

.PHONY: all stage1 lib stage2 cmds install clean
