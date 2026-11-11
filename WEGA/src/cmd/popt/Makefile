# popt, spopt: the peephole optimizer of cc and scc, reconstructed from /lib/popt. spopt is the same source with -DSPOPT, the objects in
# seg. Byte-identical with the originals.
#
#   make              build/nonseg/popt, build/seg/spopt
#   make old          the passes of the old compilers in build/nonsegVER, build/segv1, build/segvk (the switches: in the rules)
#   make install PREFIX=dir, make oldinstall PREFIX=dir (dir/lib/VER/)
#   make clean

CC = ccv0p
LD = ld
CFLAGS = -O
PREFIX = /
STRIP = strip
LIBC = ../../../../lib/libc-v5.a
CRT0 = /lib/crt0.o

NONSEG_OBJS = z20.o z21.o z22.o z23.o
SEG_OBJS = $(NONSEG_OBJS)
ALL_TARGETS = build/nonseg/popt build/seg/spopt
OLD_TARGETS = build/nonsegv0/popt build/nonsegv0p/popt build/nonsegv1/popt build/segv1/spopt build/segvk/spopt build/segvc/spopt

all: $(ALL_TARGETS)

old: $(OLD_TARGETS)

build/nonseg/popt: FORCE
	cd build/nonseg    ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile popt
build/nonsegv0/popt: FORCE
	cd build/nonsegv0  ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile popt CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS)"
build/nonsegv0p/popt: FORCE
	cd build/nonsegv0p ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile popt CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS)"
build/nonsegv1/popt: FORCE
	cd build/nonsegv1  ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile popt CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_D20"

build/seg/spopt: FORCE
	cd build/seg   ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile spopt CFLAGS="$(CFLAGS) -DSPOPT"
build/segv1/spopt: FORCE
	cd build/segv1 ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile spopt CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DSPOPT -DOLD_D20"
build/segvk/spopt: FORCE
	cd build/segvk ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile spopt CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DSPOPT -DOLD_D29"
build/segvc/spopt: FORCE
	cd build/segvc ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) -f ../../Makefile spopt CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DSPOPT -DOLD_D13 -DOLD_D45 -DOLD_D20S"

clean:
	for d in $(ALL_TARGETS) $(OLD_TARGETS) ; do\
		dir=`dirname $$d`;\
		FILES="$$dir/popt $$dir/spopt";\
		for f in $(NONSEG_OBJS) $(SEG_OBJS); do FILES="$$FILES $$dir/$$f"; done;\
		rm -f $$FILES;\
	done
	rm -f crt0.o crt0.s

install: $(ALL_TARGETS)
	cp build/nonseg/popt $(PREFIX)/lib/popt
	$(STRIP) $(PREFIX)/lib/popt
	cp build/seg/spopt   $(PREFIX)/lib/spopt
	$(STRIP) $(PREFIX)/lib/spopt

oldinstall: $(OLD_TARGETS)
	for v in v0 v0p v1 ; do \
		[ -d $(PREFIX)/lib/$$v ] || mkdir $(PREFIX)/lib/$$v ; \
		cp build/nonseg$$v/popt $(PREFIX)/lib/$$v/popt ; \
	done
	cp build/segv1/spopt $(PREFIX)/lib/v1/spopt
	[ -d $(PREFIX)/lib/vk ] || mkdir $(PREFIX)/lib/vk
	cp build/segvk/spopt $(PREFIX)/lib/vk/spopt
	[ -d $(PREFIX)/lib/vc ] || mkdir $(PREFIX)/lib/vc
	cp build/segvc/spopt $(PREFIX)/lib/vc/spopt

##################
# bin targets
##################

popt: $(NONSEG_OBJS)
	$(LD) -X -e start -i -o $@ $(CRT0) $(NONSEG_OBJS) $(LIBC)

spopt: $(SEG_OBJS)
	$(LD) -X -e start -i -o $@ $(CRT0) $(SEG_OBJS)    $(LIBC)

###################
# obj targets
###################

$(NONSEG_OBJS): ../../z2.h ../../stdio.h

z20.o: ../../z20.c
	$(CC) $(CFLAGS) -c ../../z20.c
z21.o: ../../z21.c
	$(CC) $(CFLAGS) -c ../../z21.c
z22.o: ../../z22.c
	$(CC) $(CFLAGS) -c ../../z22.c
z23.o: ../../z23.c
	$(CC) $(CFLAGS) -c ../../z23.c

FORCE:

.PHONY: all install clean old oldinstall
