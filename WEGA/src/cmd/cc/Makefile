# cc and scc, the drivers. cc.c is the source of both: scc is cc.c compiled with -DSCC.
#
#   make              build ./cc and ./scc
#   make old          the drivers of the old compiler versions in OLDDIRS (v1/Makefile: ccv1)
#   make install PREFIX=dir
#   make clean

CC = cc
LD = ld
CFLAGS = -O
PREFIX = /
LIBC = ../../../../lib/libc-v8.a
CRT0 = /lib/crt0.o

NONSEG_OBJS = cc.o
SEG_OBJS = cc.o
ALL_TARGETS = build/nonseg/cc build/seg/scc
OLD_TARGETS = build/nonsegv0/cc build/nonsegv0p/cc build/nonsegv1/cc build/nonsegv1q/cc build/nonsegv1r/cc build/segv1/scc

all: $(ALL_TARGETS)

old: $(OLD_TARGETS)

build/nonseg/cc:
	cd build/nonseg    && make -f ../../Makefile cc
build/nonsegv0/cc:
	cd build/nonsegv0  && make -f ../../Makefile cc CC="cc" CFLAGS="$(CFLAGS) -DOLD_V0"
build/nonsegv0p/cc:
	cd build/nonsegv0p && make -f ../../Makefile cc CC="cc" CFLAGS="$(CFLAGS) -DOLD_V0P"
build/nonsegv1/cc:
	cd build/nonsegv1  && make -f ../../Makefile cc CC="cc" CFLAGS="$(CFLAGS) -DOLD_V1"
build/nonsegv1q/cc:
	cd build/nonsegv1q && make -f ../../Makefile cc CC="cc" CFLAGS="$(CFLAGS) -DOLD_V1Q"
build/nonsegv1r/cc:
	cd build/nonsegv1r && make -f ../../Makefile cc CC="cc" CFLAGS="$(CFLAGS) -DOLD_V1R"

build/seg/scc:
	cd build/seg   && make -f ../../Makefile scc CFLAGS="$(CFLAGS) -DSCC"
build/segv1/scc:
	cd build/segv1 && make -f ../../Makefile scc CC="cc" CFLAGS="$(CFLAGS) -DSCC -DOLD_V1"

clean:
	for d in $(ALL_TARGETS) $(OLD_TARGETS) ; do\
		dir=`dirname $$d`;\
		FILES="$$dir/cc $$dir/scc";\
		for f in $(NONSEG_OBJS) $(SEG_OBJS); do FILES="$$FILES $$dir/$$f"; done;\
		rm -f $$FILES;\
	done

install: $(ALL_TARGETS)
	cp build/nonseg/cc $(PREFIX)/bin/cc
	cp build/seg/scc   $(PREFIX)/bin/scc

oldinstall: $(OLD_TARGETS)
	for v in v0 v0p v1 v1q v1r; do \
		cp build/nonseg$$v/cc $(PREFIX)/bin/cc$$v ; \
	done
	cp build/segv1/scc $(PREFIX)/bin/sccv1

##################
# bin targets
##################

cc: $(NONSEG_OBJS)
	$(LD) -s -e start -o $@ $(CRT0) $(NONSEG_OBJS) $(LIBC)

scc: $(SEG_OBJS)
	$(LD) -s -e start -o $@ $(CRT0) $(SEG_OBJS)    $(LIBC)

###################
# obj targets
###################

cc.o: ../../cc.c
	$(CC) $(CFLAGS) -c ../../cc.c

.PHONY: all install clean old oldinstall
