# cc and scc, the drivers. cc.c is the source of both: scc is cc.c compiled with -DSCC.
#
#   make              build/nonseg/cc, build/seg/scc
#   make old          the drivers of the old compilers in build/nonsegVER, build/segv1 (sccv1)
#   make install PREFIX=dir, make oldinstall PREFIX=dir (dir/bin/ccVER, sccv1)
#   make clean

CC = cc
LD = ld
CFLAGS = -O
PREFIX = /
STRIP = strip
LIBC = ../../../../lib/libc-v8.a
CRT0 = /lib/crt0.o

NONSEG_OBJS = cc.o
SEG_OBJS = cc.o
ALL_TARGETS = build/nonseg/cc build/seg/scc
OLD_TARGETS = build/nonsegv0/cc build/nonsegv0p/cc build/nonsegv1/cc build/nonsegv1q/cc build/nonsegv1r/cc build/segv1/scc

all: $(ALL_TARGETS)

old: $(OLD_TARGETS)

build/nonseg/cc: FORCE
	cd build/nonseg    ; $(MAKE) -f ../../Makefile cc
build/nonsegv0/cc: FORCE
	cd build/nonsegv0  ; $(MAKE) -f ../../Makefile cc CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V0"
build/nonsegv0p/cc: FORCE
	cd build/nonsegv0p ; $(MAKE) -f ../../Makefile cc CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V0P"
build/nonsegv1/cc: FORCE
	cd build/nonsegv1  ; $(MAKE) -f ../../Makefile cc CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V1"
build/nonsegv1q/cc: FORCE
	cd build/nonsegv1q ; $(MAKE) -f ../../Makefile cc CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V1Q"
build/nonsegv1r/cc: FORCE
	cd build/nonsegv1r ; $(MAKE) -f ../../Makefile cc CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DOLD_V1R"

build/seg/scc: FORCE
	cd build/seg   ; $(MAKE) -f ../../Makefile scc CFLAGS="$(CFLAGS) -DSCC"
build/segv1/scc: FORCE
	cd build/segv1 ; $(MAKE) -f ../../Makefile scc CC="cc" LIBC="$(LIBC)" CFLAGS="$(CFLAGS) -DSCC -DOLD_V1"

clean:
	for d in $(ALL_TARGETS) $(OLD_TARGETS) ; do\
		dir=`dirname $$d`;\
		FILES="$$dir/cc $$dir/scc";\
		for f in $(NONSEG_OBJS) $(SEG_OBJS); do FILES="$$FILES $$dir/$$f"; done;\
		rm -f $$FILES;\
	done

install: $(ALL_TARGETS)
	cp build/nonseg/cc $(PREFIX)/bin/cc
	$(STRIP) $(PREFIX)/bin/cc
	cp build/seg/scc   $(PREFIX)/bin/scc
	$(STRIP) $(PREFIX)/bin/scc

oldinstall: $(OLD_TARGETS)
	for v in v0 v0p v1 v1q v1r; do \
		cp build/nonseg$$v/cc $(PREFIX)/bin/cc$$v ; \
	done
	cp build/segv1/scc $(PREFIX)/bin/sccv1

##################
# bin targets
##################

cc: $(NONSEG_OBJS)
	$(LD) -e start -o $@ $(CRT0) $(NONSEG_OBJS) $(LIBC)

scc: $(SEG_OBJS)
	$(LD) -e start -o $@ $(CRT0) $(SEG_OBJS)    $(LIBC)

###################
# obj targets
###################

cc.o: ../../cc.c
	$(CC) $(CFLAGS) -c ../../cc.c

FORCE:

.PHONY: all install clean old oldinstall
