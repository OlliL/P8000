# libm: /lib/libm.a and /lib/slibm.a (the math library, nonsegmented and segmented), from the System III sources
# (tmp/sys3/usr/src/lib/libm) and fputil.s (PLZ/ASM, no source in System III or BSD: written from the original members,
# fputil.s nonsegmented, sfputil.s segmented; both identical with the shipped as -f). Both libraries are made from the
# same sources (their Library.SC are equal, 01/18/83 and 01/19/83) by the compiler of that time, v1 (ccv1, sccv1 from
# OLDROOT; with OLD_D61 and OLD_D62, host/OLDCC.md). WEGA changed exp.c (maxf 709), jn.c (sign of jn for a negative x),
# pow.c (only the form): compare/libm/SYSIII.md. Both archives are byte-identical.
#
#   make        libm.a slibm.a
#   make install PREFIX=dir   dir/lib/libm.a, dir/lib/slibm.a
#   make clean

CCV1 = ccv1
SCCV1 = sccv1
CAS = cas
AS = as
PREFIX = /
CFLAGS = -O
COBJS = asin.o atan.o fmod.o gamma.o hypot.o jn.o j0.o j1.o pow.o log.o sin.o tan.o tanh.o sinh.o exp.o

all: build/nonseg/libm.a build/seg/slibm.a

build/nonseg/libm.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CCV1) FPUTIL=fputil.s -f ../../Makefile libm.a

build/seg/slibm.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCCV1) FPUTIL=sfputil.s -f ../../Makefile slibm.a

install: all
	cp build/nonseg/libm.a $(PREFIX)/lib/libm.a
	cp build/seg/slibm.a $(PREFIX)/lib/slibm.a

clean:
	cd build/nonseg ; rm -f libm.a  arhdr.o $(COBJS) fputil.o Library.SC
	cd build/seg    ; rm -f slibm.a arhdr.o $(COBJS) fputil.o Library.SC

##################
# bin targets
##################

libm.a: $(COBJS) fputil.o ../../arhdr_libm.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libm.s
	sh ../../mkar.sh libm.a arhdr.o $(COBJS) fputil.o Library.SC

slibm.a: $(COBJS) fputil.o ../../arhdr_slibm.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibm.s
	sh ../../mkar.sh slibm.a arhdr.o $(COBJS) fputil.o Library.SC

###################
# obj targets
###################

fputil.o: ../../$(FPUTIL)
	$(AS) -f -o fputil.o ../../$(FPUTIL)
asin.o: ../../asin.c
	$(CC) $(CFLAGS) -c ../../asin.c
atan.o: ../../atan.c
	$(CC) $(CFLAGS) -c ../../atan.c
fmod.o: ../../fmod.c
	$(CC) $(CFLAGS) -c ../../fmod.c
gamma.o: ../../gamma.c
	$(CC) $(CFLAGS) -c ../../gamma.c
hypot.o: ../../hypot.c
	$(CC) $(CFLAGS) -c ../../hypot.c
jn.o: ../../jn.c
	$(CC) $(CFLAGS) -c ../../jn.c
j0.o: ../../j0.c
	$(CC) $(CFLAGS) -c ../../j0.c
j1.o: ../../j1.c
	$(CC) $(CFLAGS) -c ../../j1.c
pow.o: ../../pow.c
	$(CC) $(CFLAGS) -c ../../pow.c
log.o: ../../log.c
	$(CC) $(CFLAGS) -c ../../log.c
sin.o: ../../sin.c
	$(CC) $(CFLAGS) -c ../../sin.c
tan.o: ../../tan.c
	$(CC) $(CFLAGS) -c ../../tan.c
tanh.o: ../../tanh.c
	$(CC) $(CFLAGS) -c ../../tanh.c
sinh.o: ../../sinh.c
	$(CC) $(CFLAGS) -c ../../sinh.c
exp.o: ../../exp.c
	$(CC) $(CFLAGS) -c ../../exp.c

FORCE:

.PHONY: all install clean
