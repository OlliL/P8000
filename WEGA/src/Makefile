# WEGA src: the libraries and the commands, each with the compiler and the libc of its time.
#
#   make                  everything, in this order:
#   make stage1           the old compilers including their cas, with the shipped tools and the shipped /lib/libc.a; installed below OLDROOT.
#                         printf, fprintf, sprintf and _doprnt come from the old libc (OLDPRINTF): popt uses the value of sprintf, the
#                         buffer (the shipped one returns a count); the shipped _doprnt reads base and val of the last conversion
#                         for a %s (uninitialised: "r2" came out as "-2" behind the old printf, libl reject.c). With the old
#                         _doprnt the dummy _fegcvt of ffltioc.o (from /lib/libc.a) is linked in front of libc.a, otherwise the
#                         first _fegcvt of libc.a, the real fltioc.o with the floating point output, comes in (text overflow of
#                         the vc scodgen); the shipped _doprnt.o stands behind fltioc.o in libc.a and gets them from ffltioc.o.
#   make lib              lib/libc/libc.a and lib/libc-vN.a, with the shipped tools and the old compilers
#   make stage2           the old compilers once more, now with the libc of their time; installed below OLDROOT
#   make cmds             cmd/
#   make uts              the kernel (uts/)
#   make install          the shipped programs and libc.a of cmd/ and lib/, stripped as the originals, and the kernel (uts/), below PREFIX
#   make clean
#
# OLDROOT: where the old compilers go (bin/ccvN, lib/vN/, lib/old/cas). PREFIX: the root of make install.

CC = cc
CAS = cas
PREFIX = /
OLDROOT = /
OLDPRINTF = ../../../../lib/oldlibc/printf/s1p.o ../../../../lib/oldlibc/doprnt/s1d.o ../../../../lib/oldlibc/doprnt/s1f.o
MAKEARGS =

all: stage1 lib stage2 cmds uts

stage1: clean
	cd lib/oldlibc/printf ; cp 0_5.c s1p.c ; $(CC) -O -c s1p.c ; rm -f s1p.c
	cd lib/oldlibc/doprnt ; cp 0_5.s s1d.s ; $(CAS) -u -o s1d.o s1d.s ; rm -f s1d.s
	cd lib/oldlibc/doprnt ; ar x /lib/libc.a ffltioc.o ; mv ffltioc.o s1f.o
	cd cmd/cas    ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT)

	cd cmd/cparse ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='$(OLDPRINTF) /lib/libc.a'
	cd cmd/codgen ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='$(OLDPRINTF) /lib/libc.a'
	cd cmd/popt   ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='$(OLDPRINTF) /lib/libc.a'
	cd cmd/cc     ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) oldinstall PREFIX=$(OLDROOT) LIBC='$(OLDPRINTF) /lib/libc.a'

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

uts:
	cd uts ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) all

install:
	cd lib ; $(MAKE) install PREFIX=$(PREFIX)
	cd cmd ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) install PREFIX=$(PREFIX)
	cd etc ; $(MAKE) install PREFIX=$(PREFIX)
	cd uts ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) install PREFIX=$(PREFIX)

clean:
	cd lib ; $(MAKE) clean
	cd cmd ; $(MAKE) clean
	cd uts ; $(MAKE) clean

FORCE:

.PHONY: all stage1 lib stage2 cmds uts install clean
