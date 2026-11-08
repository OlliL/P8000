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

all: stage1 lib stage2 cmds

stage1: clean
	cd lib/oldlibc/printf ; cp 0_5.c s1.c ; $(CC) -O -c s1.c ; rm -f s1.c
	cd cmd/cas    ; $(MAKE) oldinstall PREFIX=$(OLDROOT)

	(cd cmd/cparse && $(MAKE) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a') & p1=$$!; \
	(cd cmd/codgen && $(MAKE) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a') & p2=$$!; \
	(cd cmd/popt   && $(MAKE) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a') & p3=$$!; \
	(cd cmd/cc     && $(MAKE) oldinstall PREFIX=$(OLDROOT) LIBC='../../../../$(OLDPRINTF) /lib/libc.a') & p4=$$!; \
	status=0; \
	wait $$p1 || status=1; \
	wait $$p2 || status=1; \
	wait $$p3 || status=1; \
	wait $$p4 || status=1; \
	test $$status -eq 0

lib: FORCE
	cd lib ; $(MAKE)

stage2:
	(cd cmd/cparse && $(MAKE) clean && $(MAKE) old PREFIX=$(OLDROOT)) & p1=$$!; \
	(cd cmd/codgen && $(MAKE) clean && $(MAKE) old PREFIX=$(OLDROOT)) & p2=$$!; \
	(cd cmd/popt   && $(MAKE) clean && $(MAKE) old PREFIX=$(OLDROOT)) & p3=$$!; \
	(cd cmd/cc     && $(MAKE) clean && $(MAKE) old PREFIX=$(OLDROOT)) & p4=$$!; \
	status=0; \
	wait $$p1 || status=1; \
	wait $$p2 || status=1; \
	wait $$p3 || status=1; \
	wait $$p4 || status=1; \
	test $$status -eq 0
	cd cmd/cparse ; $(MAKE) oldinstall PREFIX=$(OLDROOT)
	cd cmd/codgen ; $(MAKE) oldinstall PREFIX=$(OLDROOT)
	cd cmd/popt   ; $(MAKE) oldinstall PREFIX=$(OLDROOT)
	cd cmd/cc     ; $(MAKE) oldinstall PREFIX=$(OLDROOT)

cmds:
	cd cmd ; $(MAKE) all

install:
	cd lib ; $(MAKE) install PREFIX=$(PREFIX)
	cd cmd ; $(MAKE) install PREFIX=$(PREFIX)

clean:
	cd lib ; $(MAKE) clean
	cd cmd ; $(MAKE) clean

FORCE:

.PHONY: all stage1 lib stage2 cmds install clean
