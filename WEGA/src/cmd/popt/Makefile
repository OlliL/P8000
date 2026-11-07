# popt, spopt: the peephole optimizer of cc and scc, reconstructed from /lib/popt. spopt is the same source with -DSPOPT, the objects in
# seg. Byte-identical with the originals.
#
#   make              popt
#   make old          the versions of the old compiler in OLDDIRS (v1/Makefile ...)
#   make clean

CC = ccv0p
LD = ld
CFLAGS = -O
PREFIX = /
LIBC = ../../../../lib/libc-v5.a
CRT0 = /lib/crt0.o

NONSEG_OBJS = z20.o z21.o z22.o z23.o
SEG_OBJS = $(NONSEG_OBJS)
ALL_TARGETS = build/nonseg/popt build/seg/spopt
OLD_TARGETS = build/nonsegv0/popt build/nonsegv0p/popt build/nonsegv1/popt build/segv1/spopt

all: $(ALL_TARGETS)

old: $(OLD_TARGETS)

build/nonseg/popt:
	cd build/nonseg    && make -f ../../Makefile popt
build/nonsegv0/popt:
	cd build/nonsegv0  && make -f ../../Makefile popt CC="cc" CFLAGS="$(CFLAGS)"
build/nonsegv0p/popt:
	cd build/nonsegv0p && make -f ../../Makefile popt CC="cc" CFLAGS="$(CFLAGS)"
build/nonsegv1/popt:
	cd build/nonsegv1  && make -f ../../Makefile popt CC="cc" CFLAGS="$(CFLAGS) -DOLD_D20"

build/seg/spopt:
	cd build/seg   && make -f ../../Makefile spopt CFLAGS="$(CFLAGS) -DSPOPT"
build/segv1/spopt:
	cd build/segv1 && make -f ../../Makefile spopt CC="cc" CFLAGS="$(CFLAGS) -DSPOPT -DOLD_D20"

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
	cp build/seg/spopt   $(PREFIX)/lib/spopt

oldinstall: $(OLD_TARGETS)
	for v in v0 v0p v1 ; do \
		[ -d $(PREFIX)/lib/$$v ] || mkdir $(PREFIX)/lib/$$v ; \
		cp build/nonseg$$v/popt $(PREFIX)/lib/$$v/popt ; \
	done
	cp build/segv1/spopt $(PREFIX)/lib/v1/spopt

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

z20.o: ../../z20.c
	$(CC) $(CFLAGS) -c ../../z20.c
z21.o: ../../z21.c
	$(CC) $(CFLAGS) -c ../../z21.c
z22.o: ../../z22.c
	$(CC) $(CFLAGS) -c ../../z22.c
z23.o: ../../z23.c
	$(CC) $(CFLAGS) -c ../../z23.c

.PHONY: all install clean old oldinstall
