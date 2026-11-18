# libPW: /usr/lib/libPW.a and /usr/lib/slibPW.a (the PWB library of SCCS, nonsegmented and segmented), from the System III
# sources (tmp/sys3/usr/src/lib/libPW; move.c after its pdp11/move.c). Both libraries are made from the same sources
# (Library.SC 07/22/83) by v1 (ccv1, sccv1 from OLDROOT); the segmented one with -DSEG. WEGA passes the first
# arguments in registers: cat, regcmp and regex copy them into an array (as printf does); xalloc grows a segment of
# its own with ssgbrk in slibPW (compare/libPW/SYSIII.md). Headers: arhdr_libPW.s, arhdr_slibPW.s, mkar.sh.
#
#   make        libPW.a slibPW.a
#   make install PREFIX=dir   dir/usr/lib/libPW.a, dir/usr/lib/slibPW.a
#   make clean

CCV1 = ccv1
SCCV1 = sccv1
CAS = cas
PREFIX = /
CFLAGS = -O
OBJS = anystr.o bal.o curdir.o fdfopen.o giveup.o imatch.o index.o lockit.o logname.o move.o patoi.o patol.o \
	regcmp.o regex.o rename.o repeat.o repl.o satoi.o setsig.o sname.o strend.o substr.o trnslat.o userdir.o \
	username.o verify.o any.o xcreat.o xlink.o xopen.o xpipe.o xunlink.o xwrite.o xmsg.o cat.o dname.o xalloc.o \
	fatal.o clean.o userexit.o zero.o zeropad.o

all: build/nonseg/libPW.a build/seg/slibPW.a

build/nonseg/libPW.a: FORCE
	cd build/nonseg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(CCV1) -f ../../Makefile libPW.a

build/seg/slibPW.a: FORCE
	cd build/seg ; $(MAKE) $(MAKEARGS) MAKEARGS=$(MAKEARGS) CC=$(SCCV1) CFLAGS="$(CFLAGS) -DSEG" -f ../../Makefile slibPW.a

install: all
	-mkdir $(PREFIX)/usr $(PREFIX)/usr/lib
	cp build/nonseg/libPW.a $(PREFIX)/usr/lib/libPW.a
	cp build/seg/slibPW.a $(PREFIX)/usr/lib/slibPW.a

clean:
	cd build/nonseg ; rm -f libPW.a  arhdr.o $(OBJS) Library.SC
	cd build/seg    ; rm -f slibPW.a arhdr.o $(OBJS) Library.SC

##################
# bin targets
##################

libPW.a: $(OBJS) ../../arhdr_libPW.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_libPW.s
	sh ../../mkar.sh libPW.a arhdr.o $(OBJS) Library.SC

slibPW.a: $(OBJS) ../../arhdr_slibPW.s ../../mkar.sh ../../Library.SC
	cp ../../Library.SC ./Library.SC
	$(CAS) -u -o arhdr.o ../../arhdr_slibPW.s
	sh ../../mkar.sh slibPW.a arhdr.o $(OBJS) Library.SC

###################
# obj targets
###################

anystr.o: ../../anystr.c
	$(CC) $(CFLAGS) -c ../../anystr.c
bal.o: ../../bal.c
	$(CC) $(CFLAGS) -c ../../bal.c
curdir.o: ../../curdir.c
	$(CC) $(CFLAGS) -c ../../curdir.c
fdfopen.o: ../../fdfopen.c
	$(CC) $(CFLAGS) -c ../../fdfopen.c
giveup.o: ../../giveup.c
	$(CC) $(CFLAGS) -c ../../giveup.c
imatch.o: ../../imatch.c
	$(CC) $(CFLAGS) -c ../../imatch.c
index.o: ../../index.c
	$(CC) $(CFLAGS) -c ../../index.c
lockit.o: ../../lockit.c
	$(CC) $(CFLAGS) -c ../../lockit.c
logname.o: ../../logname.c
	$(CC) $(CFLAGS) -c ../../logname.c
move.o: ../../move.c
	$(CC) $(CFLAGS) -c ../../move.c
patoi.o: ../../patoi.c
	$(CC) $(CFLAGS) -c ../../patoi.c
patol.o: ../../patol.c
	$(CC) $(CFLAGS) -c ../../patol.c
regcmp.o: ../../regcmp.c
	$(CC) $(CFLAGS) -c ../../regcmp.c
regex.o: ../../regex.c
	$(CC) $(CFLAGS) -c ../../regex.c
rename.o: ../../rename.c
	$(CC) $(CFLAGS) -c ../../rename.c
repeat.o: ../../repeat.c
	$(CC) $(CFLAGS) -c ../../repeat.c
repl.o: ../../repl.c
	$(CC) $(CFLAGS) -c ../../repl.c
satoi.o: ../../satoi.c
	$(CC) $(CFLAGS) -c ../../satoi.c
setsig.o: ../../setsig.c
	$(CC) $(CFLAGS) -c ../../setsig.c
sname.o: ../../sname.c
	$(CC) $(CFLAGS) -c ../../sname.c
strend.o: ../../strend.c
	$(CC) $(CFLAGS) -c ../../strend.c
substr.o: ../../substr.c
	$(CC) $(CFLAGS) -c ../../substr.c
trnslat.o: ../../trnslat.c
	$(CC) $(CFLAGS) -c ../../trnslat.c
userdir.o: ../../userdir.c
	$(CC) $(CFLAGS) -c ../../userdir.c
username.o: ../../username.c
	$(CC) $(CFLAGS) -c ../../username.c
verify.o: ../../verify.c
	$(CC) $(CFLAGS) -c ../../verify.c
any.o: ../../any.c
	$(CC) $(CFLAGS) -c ../../any.c
xcreat.o: ../../xcreat.c
	$(CC) $(CFLAGS) -c ../../xcreat.c
xlink.o: ../../xlink.c
	$(CC) $(CFLAGS) -c ../../xlink.c
xopen.o: ../../xopen.c
	$(CC) $(CFLAGS) -c ../../xopen.c
xpipe.o: ../../xpipe.c
	$(CC) $(CFLAGS) -c ../../xpipe.c
xunlink.o: ../../xunlink.c
	$(CC) $(CFLAGS) -c ../../xunlink.c
xwrite.o: ../../xwrite.c
	$(CC) $(CFLAGS) -c ../../xwrite.c
xmsg.o: ../../xmsg.c
	$(CC) $(CFLAGS) -c ../../xmsg.c
cat.o: ../../cat.c
	$(CC) $(CFLAGS) -c ../../cat.c
dname.o: ../../dname.c
	$(CC) $(CFLAGS) -c ../../dname.c
xalloc.o: ../../xalloc.c
	$(CC) $(CFLAGS) -c ../../xalloc.c
fatal.o: ../../fatal.c
	$(CC) $(CFLAGS) -c ../../fatal.c
clean.o: ../../clean.c
	$(CC) $(CFLAGS) -c ../../clean.c
userexit.o: ../../userexit.c
	$(CC) $(CFLAGS) -c ../../userexit.c
zero.o: ../../zero.c
	$(CC) $(CFLAGS) -c ../../zero.c
zeropad.o: ../../zeropad.c
	$(CC) $(CFLAGS) -c ../../zeropad.c

FORCE:

.PHONY: all install clean
